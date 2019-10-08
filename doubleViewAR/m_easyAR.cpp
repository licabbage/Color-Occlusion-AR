
#include "m_easyAR.h"


GLuint loadTex(std::shared_ptr<easyar::Frame> frame);
Mat Frame2Mat(std::shared_ptr<easyar::Frame> frame);
Mat HSVBin(Mat img);
vector<vector<Point>> getContours(Mat img);

std::shared_ptr<easyar::CameraDevice> camera;
std::shared_ptr<easyar::CameraFrameStreamer> streamer;
std::vector<std::shared_ptr<easyar::ImageTracker>> trackers;
std::unordered_map<int, std::shared_ptr<easyar::ImageTarget>> trackedTargets;

std::shared_ptr<easyar::VideoPlayer> videoPlayer;

void loadFromImage(std::shared_ptr<easyar::ImageTracker> tracker, const std::string& path)
{
	auto target = std::make_shared<easyar::ImageTarget>();
	std::string jstr = "{\n"
		"  \"images\" :\n"
		"  [\n"
		"    {\n"
		"      \"image\" : \"" + path + "\",\n"
		"      \"name\" : \"" + path.substr(0, path.find_first_of(".")) + "\"\n"
		"    }\n"
		"  ]\n"
		"}";
	target->setup(jstr.c_str(), static_cast<int>(easyar::StorageType::Assets) | static_cast<int>(easyar::StorageType::Json), "");
	tracker->loadTarget(target, [](std::shared_ptr<easyar::Target> target, bool status) {
		std::printf("load target (%d): %s (%d)\n", status, target->name().c_str(), target->runtimeID());
	});
}


bool initialize()
{
	camera = std::make_shared<easyar::CameraDevice>();
	streamer = std::make_shared<easyar::CameraFrameStreamer>();
	streamer->attachCamera(camera);
	
	videoPlayer = std::make_shared<easyar::VideoPlayer>();
	//videoPlayer->setRenderTexture();
	
	
	bool status = true;
	status &= camera->open(static_cast<int>(easyar::CameraDeviceType::Default));
	camera->setSize(easyar::Vec2I{ {640, 480} });

	if (!status) { return status; }
	auto tracker = std::make_shared<easyar::ImageTracker>();
	tracker->attachStreamer(streamer);
	loadFromImage(tracker, "idback.jpg");
	trackers.push_back(tracker);
	status &= start();

	return status;

}

void finalize()
{
	trackedTargets.clear();
	trackers.clear();
	streamer = nullptr;
	camera = nullptr;
}


bool start()
{
	bool status = true;
	status &= camera->start();
	status &= streamer->start();
	camera->setFocusMode(easyar::CameraDeviceFocusMode::Continousauto);
	for (auto && tracker : trackers) {
		status &= tracker->start();
	}
	return status;
}

bool stop()
{
	bool status = true;
	for (auto && tracker : trackers) {
		status &= tracker->stop();
	}
	status &= streamer->stop();
	status &= camera->stop();
	return status;
}

void nextFrame()
{
	auto frame = streamer->peek();

	auto lostCandidates = trackedTargets;

	for (auto && targetInstance : frame->targetInstances()) {
		auto status = targetInstance->status();
		auto imagetarget = std::dynamic_pointer_cast<easyar::ImageTarget>(targetInstance->target());
		if (!imagetarget) {
			continue;
		}
		if (status == easyar::TargetStatus::Tracked) {
			if (trackedTargets.count(imagetarget->runtimeID()) == 0) {
				trackedTargets[imagetarget->runtimeID()] = imagetarget;
				std::printf("found target: %s (%d)\n", imagetarget->name().c_str(), imagetarget->runtimeID());
			}
			lostCandidates.erase(imagetarget->runtimeID());

			auto pose = targetInstance->poseGL();
			std::printf("target: %s (%d) %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", imagetarget->name().c_str(), imagetarget->runtimeID(), pose.data[0], pose.data[1], pose.data[2], pose.data[3], pose.data[4], pose.data[5], pose.data[6], pose.data[7], pose.data[8], pose.data[9], pose.data[10], pose.data[11], pose.data[12], pose.data[13], pose.data[14], pose.data[15]);
		}
	}

	for (auto p : lostCandidates) {
		auto imagetarget = std::get<1>(p);
		if (trackedTargets.count(imagetarget->runtimeID()) > 0) {
			trackedTargets.erase(imagetarget->runtimeID());
			std::printf("lost target: %s (%d)\n", imagetarget->name().c_str(), imagetarget->runtimeID());
		}
	}
}


std::shared_ptr<easyar::Frame> m_nextFrame(GLuint &texID, float matrix[16])
{
	auto frame = streamer->peek();
	texID =  loadTex(frame);
	auto lostCandidates = trackedTargets;
	
	for (auto && targetInstance : frame->targetInstances()) {
		auto status = targetInstance->status();
		auto imagetarget = std::dynamic_pointer_cast<easyar::ImageTarget>(targetInstance->target());
		if (!imagetarget) {
			continue;
		}
		if (status == easyar::TargetStatus::Tracked) {
			if (trackedTargets.count(imagetarget->runtimeID()) == 0) {
				trackedTargets[imagetarget->runtimeID()] = imagetarget;
				//std::printf("found target: %s (%d)\n", imagetarget->name().c_str(), imagetarget->runtimeID());
			}
			lostCandidates.erase(imagetarget->runtimeID());
			auto pose = targetInstance->poseGL();
			
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					matrix[i*4 +j] = pose.data[i*4+j];
				}
			}
			//std::printf("target: %s (%d) %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", imagetarget->name().c_str(), imagetarget->runtimeID(), pose.data[0], pose.data[1], pose.data[2], pose.data[3], pose.data[4], pose.data[5], pose.data[6], pose.data[7], pose.data[8], pose.data[9], pose.data[10], pose.data[11], pose.data[12], pose.data[13], pose.data[14], pose.data[15]);
		}
	}

	for (auto p : lostCandidates) {
		auto imagetarget = std::get<1>(p);
		if (trackedTargets.count(imagetarget->runtimeID()) > 0) {
			trackedTargets.erase(imagetarget->runtimeID());
			std::printf("lost target: %s (%d)\n", imagetarget->name().c_str(), imagetarget->runtimeID());
		}
	}

	return frame;
}

GLuint loadTex(std::shared_ptr<easyar::Frame> frame)
{
	
	auto image = frame->images().back();
	GLuint last_texture_ID = 0, texture_ID = 0;

	glGenTextures(1, &texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,image->width() , image->height(), 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image->data());
	

	//纹理放大缩小使用线性插值
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return texture_ID;
}


Mat Frame2Mat(std::shared_ptr<easyar::Frame> frame)
{
	auto image = frame->images().back();
	
	Mat imageMat;
	imageMat.create(image->height(), image->width(), CV_8UC3);
	imageMat.data = (uchar*)image->data();
	
	return imageMat;
}

Mat HSVBin(Mat img)
{
	Mat hsv;
	cvtColor(img, hsv, COLOR_RGB2HSV);
	Mat mask;
	inRange(hsv, Scalar(100, 50, 0), Scalar(125, 255, 255), mask);

	//return mask;
	Mat open_output;
	Mat close_output;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

	morphologyEx(mask, open_output, MORPH_OPEN, kernel);
	morphologyEx(open_output, close_output, MORPH_CLOSE, kernel);

	return close_output;
}

vector<vector<Point>> getContours(Mat img)
{
	Mat open_output;
	Mat close_output;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

	morphologyEx(img, open_output, MORPH_OPEN, kernel);
	morphologyEx(open_output, close_output, MORPH_CLOSE, kernel);

	vector<vector<Point>> allContours;
	findContours(close_output, allContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> validContours;
	for (int i = 0; i < allContours.size(); i++)
	{
		auto contour = allContours[i];
		if (contourArea(contour) > 9000)
		{
			vector<Point> hull;
			convexHull(contour, hull);
			
			validContours.push_back(hull);
		}
	}
	
	return validContours;
}


void getProjection(float projectionMat[16])
{
	auto projection =  camera->projectionGL(0.2f, 500.0f);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			projectionMat[j*4+i] = projection.data[j*4+i];
		}	
	}
	
}