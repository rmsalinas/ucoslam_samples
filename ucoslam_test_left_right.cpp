/**
* This file is part of  UCOSLAM
*
* Copyright (C) 2018 Rafael Munoz Salinas <rmsalinas at uco dot es> (University of Cordoba)
*
* UCOSLAM is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* UCOSLAM is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with UCOSLAM. If not, see <http://wwmap->gnu.org/licenses/>.
*/


/**This example shows you how to create a map using the left camera of the Kitti dataset,
 * saves it to a file, and then uses the right camera for tracking only
 */
#include <opencv2/highgui.hpp>
#include <iostream>
#include "ucoslam/ucoslam.h"
#include "ucoslam/mapviewer.h"
using namespace std;

void createMapWithLeft(string videoFile,string cameraparamsfile,string vocfilepath,string outputmap ){
    //open video
    cv::VideoCapture videoCap(videoFile);//loads video
    if(!videoCap.isOpened()) throw std::runtime_error("Could not load file "+videoFile);
    //load camera params
    ucoslam::ImageParams cameraParams;
    cameraParams.readFromXMLFile(cameraparamsfile);
    auto map=std::make_shared<ucoslam::Map>();//creates empty map
    ucoslam::Params ucoParams;
    ucoParams.runSequential=true;
    ucoParams.detectMarkers=false;
    ucoslam::UcoSlam SLAM;
    SLAM.setParams(map,ucoParams,vocfilepath);
    //lets us process the first 100 frames
    for(int i=0;i<100;i++){
        cv::Mat im;
        videoCap>>im;
        SLAM.process(im,cameraParams,i);
        cout<<"processing ..."<<i<<endl;
    }
    //let us compress the map removing unused key points [optional]
    map->removeUnUsedKeyPoints();
    //save the map
    map->saveToFile(outputmap);
}


int main(int argc,char **argv){

    try {

        if(argc!=3) throw std::runtime_error("dirToKittiDataSet orb.fbow");

        string mapName=argv[1]+string("/kitti00_left.map");
        createMapWithLeft(argv[1]+string("/cam0.mp4"),argv[1]+string("/cam0.yml"),argv[2],mapName);


        cv::VideoCapture videoCap(argv[1]+string("/cam1.mp4"));//loads right video
        if(!videoCap.isOpened()) throw std::runtime_error("Could not load file "+string(argv[1])+string("/cam1.mp4"));
        //load camera params
        ucoslam::ImageParams cameraParams;
        cameraParams.readFromXMLFile(argv[1]+string("/cam1.yml"));

        auto map=std::make_shared<ucoslam::Map>();//creates empty map
        map->readFromFile(mapName);//loads it from file

        ucoslam::Params ucoParams;
        ucoParams.runSequential=true;
        ucoParams.detectMarkers=false;
        ucoslam::UcoSlam SLAM;
        ucoslam::MapViewer Viewer;
        //sets the operating parameters
        //we do not have to specify the vocabulary since is already in the map
        SLAM.setParams(map,ucoParams);
        SLAM.setMode(ucoslam::MODE_LOCALIZATION);//localization only, no map update


        //process until ESC
        char key=0;
        cv::Mat image;
        while(videoCap.grab() && key!=27){
            videoCap.read(image);
            cv::Mat pose=SLAM.process(image,cameraParams,videoCap.get(CV_CAP_PROP_POS_FRAMES));
            key=Viewer.show(map,image,pose);

            if( key=='i'){//move the video forward 10 frames
                int pos=videoCap.get(CV_CAP_PROP_POS_FRAMES);
                pos=std::min( pos+10,int(videoCap.get(CV_CAP_PROP_FRAME_COUNT)-1));
                videoCap.set(CV_CAP_PROP_POS_FRAMES,pos);
            }
            if( key=='u'){//moves the video backwards 10 frames
                int pos=videoCap.get(CV_CAP_PROP_POS_FRAMES);
                pos=std::max( pos-10,0);
                videoCap.set(CV_CAP_PROP_POS_FRAMES,pos);
            }

        }
    } catch (std::exception &ex) {
        std::cerr<<ex.what()<<std::endl;
    }


}
