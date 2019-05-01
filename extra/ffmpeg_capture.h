#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>


#include <string>

class ffmpeg_capture
{
public:
	//const char* cmd_ = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1000x800 -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip output.mp4";

	std::string cmd_;

	int width_;
	int height_;

	FILE* ffmpeg_;
	int* buffer_;


public:
	void init(int _width, int _height, int frame_rate = 24, std::string _path = "output.mp4")
	{
		width_ = _width;
		height_ = _height;

		//"ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1000x800 -i - -threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip output.mp4";
		cmd_ = "ffmpeg";
		cmd_ += " -r " + std::to_string(frame_rate);
		cmd_ += " -f rawvideo -pix_fmt rgba ";
		cmd_ += " -s " + std::to_string(width_) + "x" + std::to_string(height_);
		cmd_ += " -i - -threads 4 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip -loglevel quiet";
		cmd_ += " " + _path;

		ffmpeg_ = popen(cmd_.c_str(), "w");
		if(ffmpeg_ == nullptr)
		{
			std::cout << "error " << std::endl;
			exit(1);
		}


		buffer_ = new int[_width * _height];
	}
	void save()
	{
		glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, buffer_);
		fwrite(buffer_, sizeof(int) * width_ * height_, 1, ffmpeg_);
	}
	void clear()
	{
		//_pclose(ffmpeg_);
		pclose(ffmpeg_);
		delete buffer_;
	}
};
