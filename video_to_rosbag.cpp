#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ros/ros.h>

int main(int argc, char** argv) {
    ros::init(argc, argv, "video_to_rosbag");
    ros::NodeHandle nh;

    // Set the path to the video file
    std::string video_file_path = "3.mp4";  // Replace with your video file

    // Open the video file
    cv::VideoCapture cap(video_file_path);

    if (!cap.isOpened()) {
        ROS_ERROR("Failed to open video file.");
        return 1;
    }

    // Initialize a CvBridge
    cv_bridge::CvImage cv_image;

    // Create a ROS bag for recording
    rosbag::Bag bag;
    bag.open("/media/kasun/0ABCA00CBC9FF105/academic/7thSem/Project/data/video3.bag", rosbag::bagmode::Write);

    // Create an ImageTransport for publishing images
    image_transport::ImageTransport it(nh);
    image_transport::Publisher image_pub = it.advertise("/camera/image_raw", 1);

    // Determine the frame rate of the video (you can manually specify it if you know it)
    double frame_rate = cap.get(cv::CAP_PROP_FPS) or 25;  // Default to 25 FPS if frame rate is unknown

    ros::Rate loop_rate(frame_rate);

    while (ros::ok()) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            break;  // End of video
        }

        // Convert the frame to an Image message
        cv_image.image = frame;
        cv_image.encoding = "bgr8";

        sensor_msgs::ImagePtr image_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();

        // Set the timestamp of the image message
        image_msg->header.stamp = ros::Time::now();

        // Publish the image message
        image_pub.publish(image_msg);

        // Write the image message to the ROS bag
        bag.write("/camera/image_raw", ros::Time::now(), image_msg);

        loop_rate.sleep();
    }

    bag.close();

    return 0;
}

