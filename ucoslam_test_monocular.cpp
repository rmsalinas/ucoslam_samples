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

/**This program shows a basic example to use UcoSLAM to process the Kitti data set using monocular images.
 *
 * You can download the dataset for evaluation at https://mega.nz/#F!YsU2AY7L!Of0oChqpFBh34Y0-GOQ7VQ
 * which are adapted to the input formats of the library
 */
#include <iostream>
#include <ucoslam/ucoslam.h>
#include <ucoslam/mapviewer.h>
#include <opencv2/highgui/highgui.hpp>
int main(int argc,char **argv){
    try {
        if(argc!=5) throw std::runtime_error("usage : video cameraparameters.yml vocabulary.fbow outputmap.map");

        cv::VideoCapture VideoIn;//video capturer
        ucoslam::UcoSlam SLAM;//The main class
        ucoslam::Params UcoSlamParams;//processing parameters
        ucoslam::ImageParams cameraParams;//camera parameters
        ucoslam::MapViewer MapViwer;//Viewer to see the 3D map and the input images

        //creates an empty map
        std::shared_ptr<ucoslam::Map> map=std::make_shared<ucoslam::Map>();
        //map->loadFromFile(pathToFile);
        //open video
        VideoIn.open(argv[1]);
        if(!VideoIn.isOpened()) throw std::runtime_error("Could not open video:"+string(argv[1]));
        //read camera params
        cameraParams.readFromXMLFile(argv[2]);
        //set the slam params for Kitti using orb descriptor
        UcoSlamParams.runSequential=true;//run in sequential mode to avoid skipping frames
        UcoSlamParams.detectMarkers=false;//no markers in this example.

        //Start UcoSlam
        SLAM.setParams(map,UcoSlamParams,argv[3]);//the last parameter is the path to the vocabulary file of extension .fbow


        cv::Mat inputImage;
        char keyPressed=0;
        while( VideoIn.grab() && keyPressed!=27){//keyPressed ==27 is esc
            VideoIn.retrieve(inputImage);
            int frameNumber=VideoIn.get(CV_CAP_PROP_POS_FRAMES);
            cv::Mat posef2g= SLAM.process(inputImage,cameraParams,frameNumber);
            if(posef2g.empty()){
                std::cerr<<"Frame "<<frameNumber<<" pose not found"<<std::endl;
            }
            else
                std::cerr<<"Frame "<<frameNumber<<" pose "<<posef2g<<std::endl;
            //draw a mininimal interface in an opencv window
            keyPressed=MapViwer.show(map,inputImage,posef2g);
        }
        //now,  save the map
        map->saveToFile(argv[4]);
    } catch (std::exception &ex) {
        std::cout<<ex.what()<<std::endl;
    }
}

