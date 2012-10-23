/*
 *   (C) 2012, Aurel Wildfellner
 *
 *   This file is part of Barabella.
 *
 *   Barabella is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Barabella is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Barabella.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include <pcl/common/transforms.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/ModelCoefficients.h>


#include "kinect_interface.h"
#include "view3d.h"
#include "utils.h"
#include "floor_extractor.h"
#include "barabella_config.h"
#include "clip.h"
#include "global_options.h"
#include "clip_player.h"
#include "icp_framed_tracker.h"


#ifndef __BARABELLA_APP_H__
#define __BARABELLA_APP_H__


class BarabellaApp {

    public:
        typedef pcl::PointXYZRGBA PointT;
        typedef pcl::PointCloud<PointT> PointCloud;
        typedef typename PointCloud::Ptr PointCloudPtr;
        typedef typename PointCloud::ConstPtr PointCloudConstPtr;

    public:

        enum OperationMode {
            STREAMING,
            CLIPPLAYBACK,
            TRACKING,
        };

        BarabellaApp(GlobalOptions* op) :
            gOptions(op),
            floorCoefficients( new pcl::ModelCoefficients()),
            displayTemplate(false)
        {
            kinIface.waitForFirstFrame();
            initView3d();
            initTemplates();
            operationMode = STREAMING;

            /* FIXME player test */
            clip = new Clip();
            clip->setDirectory(gOptions->clipDirectory);

            if (gOptions->doRecording) {
                clip->startRecording(&kinIface);
            }

            if (gOptions->doPlayBack) {
                clipPlayer.setClip(clip);
                clipPlayer.start();
                operationMode = CLIPPLAYBACK;
            }

        }

        void spinOnce();

        void updateFloor();
        void extractTemplate();
        void setOperationMode(OperationMode mode);
        void saveTemplateSettings();

        /* clip recording */
        void recordNewClip();
        void stopRecording();

    private:

        /* options */
        GlobalOptions* gOptions;
        bool displayTemplate;
        OperationMode operationMode;

        /* main modules */
        View3D view3d;
        KinectInterface kinIface;
        SelectionCube sCube;
        FloorExtractor floorEx;

        /* cloud data */
        PointCloudConstPtr mainCloud;
        Eigen::Affine3f floorTrans;
        pcl::ModelCoefficients::Ptr floorCoefficients;

        /* init */
        void initView3d();
        void initTemplates();

        /* spins */
        void spinStreaming();
        void spinClipPlayBack();
        void spinTracking();

        /* recording */
        Clip* clip;
        ClipPlayer clipPlayer;

        /* tracking */
        PointCloudPtr templateCloud;
        IcpFramedTracker tracker;
        void startTracker();

};


#endif

