/*
 * cloud_player_widget.h
 *
 *  Created on: Jun 5, 2013
 *      Author: asher
 */

#ifndef CLOUD_PLAYER_WIDGET_H_
#define CLOUD_PLAYER_WIDGET_H_

#include "ui_cloud_player.h"
#include <pcl/io/grabber.h>
#include <pcl/io/recorder.h>

#include <pcl/visualization/pcl_visualizer.h>

#include <QWidget>

class QErrorMessage;

namespace pcl {
namespace visualization{

class CloudRenderer  : public QObject{
	Q_OBJECT
	protected:
		std::string description_;
		QVTKWidget* widget_;
		pcl::visualization::PCLVisualizer* visualizer_;
		boost::signals2::connection connection_;
	public:
		CloudRenderer( ): widget_(NULL), visualizer_(NULL){}
		virtual ~CloudRenderer(){}

		virtual void init(QVTKWidget* widget, PCLVisualizer * visualizer){
			widget_=widget;
			visualizer_=visualizer;
		}
		virtual bool setup(boost::shared_ptr<Grabber>& grabber)=0;
		virtual void disconnect()=0;

		virtual void renderNew()=0;
		std::string description(){return description_;};

	signals:
		void update();
};

class CloudRendererRange  : public CloudRenderer{
	public:
		CloudRendererRange(std::string field);
		virtual ~CloudRendererRange(){}
		virtual bool setup(boost::shared_ptr<Grabber>& grabber);
		virtual void disconnect();
		virtual void renderNew();
	private:
		std::string field_name_;
		boost::mutex cloud_mutex_;
		sensor_msgs::PointCloud2::ConstPtr cloud_;
		bool valid_grabber_;
		void grabberCB( const sensor_msgs::PointCloud2::ConstPtr& cloud);
		boost::signals2::connection connection_;
};


class CloudPlayerWidget : public QWidget{
	Q_OBJECT

	public:
		CloudPlayerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
		virtual ~CloudPlayerWidget();

		void setGrabber(boost::shared_ptr<Grabber>& grabber);

		void addCloudRenderer(CloudRenderer* renderer);
		uint32_t  currentRendererIDX(){return current_renderer_idx_;}
		void setCurrentRenderer(uint32_t idx);
		CloudRenderer* getRenderer(size_t idx);
		size_t getNumRenderers(){return renderers_.size();}


		void addRecorder(Recorder* recorder);
		uint32_t  currentRecorderIDX(){return current_recorder_idx_;}
		void setCurrentRecorder(uint32_t idx);

	private:
		boost::shared_ptr<Grabber> grabber_;
		bool is_movie_grabber_;
		PCLVisualizer* pcl_visualizer_;

		Ui_CloudPlayer ui_;
		std::vector<CloudRenderer*> renderers_;
		std::vector<Recorder*> recorders_;

		uint32_t current_renderer_idx_;
		uint32_t current_recorder_idx_;

		bool recording_;
		bool playing_;
		boost::signals2::connection progress_connection_;

		QErrorMessage* error_msg_;

	public slots:
		void playPause( );
		void sliderValueChanged(int val);
		void record( );
		void resetView( );
		virtual  void updateCloud();
		void setRenderer(int idx);
	protected :
		void keyCB(const pcl::visualization::KeyboardEvent& e);
		void enablePlayback();
		void disablePlayback();
		void progressUpdate(size_t frame_num, size_t frame_total);
		void startRecording();
		void stopRecording();
	};
}

} /* namespace adaptive_pushgrasp */
#endif /* CLOUD_PLAYER_WIDGET_H_ */