//    *************************** LiveVisionKit ****************************
//    Copyright (C) 2022  Sebastian Di Marco (crowsinc.dev@gmail.com)
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 	  **********************************************************************

#pragma once

#include <opencv2/opencv.hpp>

namespace lvk
{

	struct CameraParameters
	{
		cv::Mat camera_matrix;
		std::vector<float> distortion_coefficients;
	};

	class CameraCalibrator
	{
	public:

		CameraCalibrator(const cv::Size& chessboard_pattern_size);

		bool feed(const cv::UMat& frame);

		CameraParameters calibrate(const uint32_t square_size = 1) const;

		void reset();

		uint32_t calibration_frames() const;

	private:

		const cv::Size m_PatternSize;

		cv::Size m_ImageSize;
		std::vector<std::vector<cv::Point2f>> m_ImagePoints;

	};

}
