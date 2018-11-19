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

/**This program shows a how to iterate in the map
 */
#include "ucoslam/ucoslam.h"
#include <iostream>
using namespace std;
int main(int argc,char **argv){
    try {

        if(argc!=2) throw std::runtime_error("Usage : map");
        auto Map=std::make_shared<ucoslam::Map>();
        Map->readFromFile(argv[1]);

        cout<<"Number of MapPoints= "<<Map->map_points.size()<<endl;
        cout<<"Number of KeyFrames= "<<Map->keyframes.size()<<endl;
        cout<<"Number of Markers= "<<Map->map_markers.size()<<endl;
        //move along all map points
        for(const ucoslam::MapPoint &point:Map->map_points){
            cout<<"Map Point:"<<point.id<<" pos="<<point.getCoordinates()<<endl;
            cout<<" Is observed in:"<<endl;
            for(auto _pair:point.getObservingFrames()){
                cout<<"     Frame idx="<<_pair.first<<" keypoint: "<<_pair.second;
                //print the keypoint 2d position
                cv::Point2f pixel=Map->keyframes[ _pair.first ].und_kpts[ _pair.second].pt;
               cout<<" at the pixel:"<<pixel<<endl;
            }
        }
        //the same in a C++98 style
        for(size_t i=0; Map->map_points.capacity();i++){
            if( Map->map_points.is(i)){//check if valid
                const ucoslam::MapPoint  &point=Map->map_points[i];
                cout<<"Map Point:"<<point.id<<" pos="<<point.getCoordinates()<<endl;
                cout<<" Is observed in:"<<endl;
                for(auto _pair:point.getObservingFrames()){
                    cout<<"     Frame idx="<<_pair.first<<" keypoint: "<<_pair.second;
                    //print the keypoint 2d position
                    cv::Point2f pixel=Map->keyframes[ _pair.first ].und_kpts[ _pair.second].pt;
                    cout<<" at the pixel:"<<pixel<<endl;
                }
            }
        }



    } catch (std::exception &ex) {
     cout<<   ex.what()<<endl;
    }
}
