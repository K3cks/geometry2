/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Tully Foote */

#ifndef TF_TRANSFORMLISTENER_H
#define TF_TRANSFORMLISTENER_H

#include "sensor_msgs/PointCloud.h"
#include "std_msgs/Empty.h"
#include "tf/tfMessage.h"
#include "tf/tf.h"
#include "ros/ros.h"
#include "ros/callback_queue.h"

#include "tf/FrameGraph.h" //frame graph service
#include "boost/thread.hpp"

namespace tf{

/** \brief Get the tf_prefix from the parameter server
 * \param nh The node handle to use to lookup the parameter.
 * \return The tf_prefix value for this NodeHandle 
 */
std::string getPrefixParam(ros::NodeHandle & nh) {
  std::string param; 
  if (!nh.searchParam("tf_prefix", param)) 
    return ""; 
  
  std::string return_val;
  nh.getParam(param, return_val);
  return return_val;
}

/** \brief resolve names \todo document me */
std::string remap(const std::string& frame_id) __attribute__((deprecated));

/** \brief This class inherits from Transformer and automatically subscribes to ROS transform messages */
class TransformListener : public Transformer { //subscribes to message and automatically stores incoming data

public:
  /**@brief Constructor for transform listener
   * \param max_cache_time How long to store transform information */
  TransformListener(ros::Duration max_cache_time = ros::Duration(DEFAULT_CACHE_TIME), bool spin_thread = true);

  /**
   * \brief Alternate constructor for transform listener
   * \param nh The NodeHandle to use for any ROS interaction
   * \param max_cache_time How long to store transform information
   */
  TransformListener(const ros::NodeHandle& nh,
                    ros::Duration max_cache_time = ros::Duration(DEFAULT_CACHE_TIME), bool spin_thread = true);
  
  ~TransformListener();

  /* Methods from transformer unhiding them here */
  using Transformer::transformQuaternion;
  using Transformer::transformVector;
  using Transformer::transformPoint;
  using Transformer::transformPose;


  /** \brief Transform a Stamped Quaternion Message into the target frame 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformQuaternion(const std::string& target_frame, const geometry_msgs::QuaternionStamped& stamped_in, geometry_msgs::QuaternionStamped& stamped_out) const;
  /** \brief Transform a Stamped Vector Message into the target frame 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformVector(const std::string& target_frame, const geometry_msgs::Vector3Stamped& stamped_in, geometry_msgs::Vector3Stamped& stamped_out) const;
  /** \brief Transform a Stamped Point Message into the target frame 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformPoint(const std::string& target_frame, const geometry_msgs::PointStamped& stamped_in, geometry_msgs::PointStamped& stamped_out) const;
  /** \brief Transform a Stamped Pose Message into the target frame 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformPose(const std::string& target_frame, const geometry_msgs::PoseStamped& stamped_in, geometry_msgs::PoseStamped& stamped_out) const;


  /** \brief Transform a Stamped Quaternion Message into the target frame
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformQuaternion(const std::string& target_frame, const ros::Time& target_time,
                           const geometry_msgs::QuaternionStamped& qin,
                           const std::string& fixed_frame, geometry_msgs::QuaternionStamped& qout) const;
  /** \brief Transform a Stamped Vector Message into the target frame and time 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformVector(const std::string& target_frame, const ros::Time& target_time,
                       const geometry_msgs::Vector3Stamped& vin,
                           const std::string& fixed_frame, geometry_msgs::Vector3Stamped& vout) const;
  /** \brief Transform a Stamped Point Message into the target frame and time  
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformPoint(const std::string& target_frame, const ros::Time& target_time,
                           const geometry_msgs::PointStamped& pin,
                           const std::string& fixed_frame, geometry_msgs::PointStamped& pout) const;
  /** \brief Transform a Stamped Pose Message into the target frame and time  
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformPose(const std::string& target_frame, const ros::Time& target_time,
                     const geometry_msgs::PoseStamped& pin,
                     const std::string& fixed_frame, geometry_msgs::PoseStamped& pout) const;


  /** \brief Transform a sensor_msgs::PointCloud natively 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
    void transformPointCloud(const std::string& target_frame, const sensor_msgs::PointCloud& pcin, sensor_msgs::PointCloud& pcout) const;

  /** @brief Transform a sensor_msgs::PointCloud in space and time 
   * This can throw all that lookupTransform can throw as well as tf::InvalidTransform */
  void transformPointCloud(const std::string& target_frame, const ros::Time& target_time,
                           const sensor_msgs::PointCloud& pcin,
                           const std::string& fixed_frame, sensor_msgs::PointCloud& pcout) const;



    ///\todo move to high precision laser projector class  void projectAndTransformLaserScan(const sensor_msgs::LaserScan& scan_in, sensor_msgs::PointCloud& pcout);

  bool getFrames(tf::FrameGraph::Request& req, tf::FrameGraph::Response& res) 
  {
    res.dot_graph = allFramesAsDot();
    return true;
  }

  std::string resolve(const std::string& frame_id)
  {
    return tf::resolve(tf_prefix_, frame_id);
  };

private:
  /// Initialize this transform listener, subscribing, advertising services, etc.
  void init();
  void initWithThread();

  /// Callback function for ros message subscriptoin
  void subscription_callback(const tf::tfMessageConstPtr& msg);

  /** @brief a helper function to be used for both transfrom pointCloud methods */
  void transformPointCloud(const std::string & target_frame, const Transform& transform, const ros::Time& target_time, const sensor_msgs::PointCloud& pcin, sensor_msgs::PointCloud& pcout) const;

  /// clear the cached data
  void reset_callback(const std_msgs::EmptyConstPtr &msg);
  std_msgs::Empty empty_;
  ros::ServiceServer tf_frames_srv_;


  ros::CallbackQueue tf_message_callback_queue_;
  boost::thread* dedicated_listener_thread_;
  ros::NodeHandle node_;
  ros::Subscriber message_subscriber_tf_, message_subscriber_tf_message_, reset_time_subscriber_;

  void dedicatedListenerThread()
  {
    while (using_dedicated_thread_)
    {
      tf_message_callback_queue_.callAvailable(ros::WallDuration(0.01));
    }
  };

};
}

#endif //TF_TRANSFORMLISTENER_H
