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

/**This program shows a basic example to use UcoSLAM to process the Kitti data set using
 * stereo images.
 * The program assumes that images are rectified and that an appropriate calibration file is provided.
 * You can download the dataset for evaluation at https://mega.nz/#F!YsU2AY7L!Of0oChqpFBh34Y0-GOQ7VQ
 * which are adapted to the input formats of the library
 */
#include <iostream>
#include <ucoslam/ucoslam.h>
#include <ucoslam/mapviewer.h>
using namespace std;

int main(int argc, char* argv[]){
    try {
        if(argc!=6)
            throw std::runtime_error("Usage: <video1> <video2> <stereo_calibration_file>  <voc_file> <outmap.map>");
        ucoslam::ImageParams image_params;
        cv::VideoCapture video[2];
        ucoslam::UcoSlam system;
        ucoslam::MapViewer mv;
        std::shared_ptr<ucoslam::Map> map=make_shared<ucoslam::Map>();


        image_params.readFromXMLFile(argv[3]);
        for(int i=0;i<2;i++){
            video[i].open(argv[i+1]);
            if(!video[i].isOpened())
                throw runtime_error(string("Cannot open video file at:")+argv[i+1]);
        }

        //prepare params for Kitti
        ucoslam::Params sparams;
        sparams.detectMarkers=false;
        sparams.runSequential=true;//this disables parrallel mapping to avoid skipping frames


        //configure the system priving the map, the operation params, and the vocabulary file (optional)
        system.setParams(map,sparams,argv[4]);
        char key=0;
        cv::Mat rectified_image[2];
        while( video[0].grab() && video[1].grab() && key!=27){
            for(int i=0;i<2;i++)
                video[i].retrieve(rectified_image[i]);
            int frameNumber=video[0].get(CV_CAP_PROP_POS_FRAMES);
            cv::Mat pose=system.processStereo(rectified_image[0],rectified_image[1],image_params,frameNumber );
            key=mv.show(map,rectified_image[0],pose);
        }

        //finally, save the map
        map->saveToFile(argv[5]);
        return 0;
    } catch (std::exception &ex) {
     cerr<<ex.what()<<endl;
    }

}
