#include <opencv2/opencv.hpp>

#include "gtest/gtest.h"

#include <fstream>
#include <string>
#include <vector>

#include "map_initializer.hpp"

class MapInitializerTest : public ::testing::Test {
 protected:
  void CreateInitializer() {
    map_initializer_ = vio::MapInitializer::CreateMapInitializer(options_);
  }

  void GetFeatureVectorFromMatchFile() {
    cv::Mat_<double> x1, x2;
    int num_pts;
    std::ifstream myfile("../../map_initializer/test/test_data/recon2v_checkerboards.txt");
    ASSERT_TRUE(myfile.is_open());

    feature_vectors_.resize(2);
    std::string line;

    // TODO: Put this to util for other use cases.
    // Read number of points
    getline(myfile, line);
    num_pts = (int)atof(line.c_str());

    feature_vectors_[0].resize(num_pts);
    feature_vectors_[1].resize(num_pts);

    x1 = cv::Mat_<double>(2, num_pts);
    x2 = cv::Mat_<double>(2, num_pts);

    // Read the point coordinates
    for (int i = 0; i < num_pts; ++i) {
   
      getline(myfile, line);
      std::stringstream s(line);
      std::string cord;
      s >> cord; x1(0, i) = atof(cord.c_str());
      s >> cord; x1(1, i) = atof(cord.c_str());
      s >> cord; x2(0, i) = atof(cord.c_str());
      s >> cord; x2(1, i) = atof(cord.c_str());

      feature_vectors_[0][i][0] = x1(0, i);
      feature_vectors_[0][i][1] = x1(1, i);
      feature_vectors_[1][i][0] = x2(0, i);
      feature_vectors_[1][i][1] = x2(1, i);
    }
    myfile.close();
    K_ = cv::Matx33d(1, 0, 0, 0, 1, 0, 0, 0, 1); 
    // K_ = cv::Matx33d(350, 0, 240, 0, 350, 360, 0, 0, 1); 
  }
/*
  void GetFeatureVectorFromTracks() {
    std::ifstream myfile("../map_initializer/test/test_data/backyard_tracks.txt");
    ASSERT_TRUE(myfile.is_open());
    double x, y;
    std::string line_str;
    int num_frames = 0, n_tracks = 0;
    std::vector<std::vector<cv::Vec2d> > tracks;
         
    for ( ; getline(myfile,line_str); ++n_tracks) {
      std::istringstream line(line_str);
      std::vector<cv::Vec2d> track;
      for ( num_frames = 0; line >> x >> y; ++num_frames) {
        if ( x > 0 && y > 0)
          track.push_back(cv::Vec2d(x,y));
        else
          track.push_back(cv::Vec2d(-1));
      }
      tracks.push_back(track);
    }
    for (int i = 0; i < num_frames; ++i) {
      cv::Mat frame(cv::Size(2, n_tracks));
      for (int j = 0; j < n_tracks; ++j)
      {
        frame(0,j) = tracks[j][i][0];
        frame(1,j) = tracks[j][i][1];
      }
      points2d.push_back(frame);
    }
    myfile.close();
    K_ = cv::Matx33d(1, 0, 0, 0, 1, 0, 0, 0, 1); 
  }
*/
  void RunInitializer() {
    std::vector<cv::Point3f> points3d;
    std::vector<bool> points3d_mask;
    std::vector<cv::Mat> Rs_est, ts_est;

    ASSERT_TRUE(map_initializer_->Initialize(feature_vectors_,
                                             cv::Mat(K_), points3d,
                                             points3d_mask,
                                             Rs_est, ts_est));
  }
  
  vio::MapInitializer *map_initializer_;
  vio::MapInitializerOptions options_;

  std::vector<std::vector<cv::Vec2d> > feature_vectors_;

  cv::Matx33d K_;
};

#ifdef CERES_FOUND
TEST_F(MapInitializerTest, TestLibmv_TwoFrame_MatchFile) {
  options_.method = vio::MapInitializerOptions::LIBMV;
  CreateInitializer();
  GetFeatureVectorFromMatchFile();

  RunInitializer();
}
#endif

TEST_F(MapInitializerTest, Test8Point_TwoFrame_MatchFile) {
  options_.method = vio::MapInitializerOptions::NORMALIZED8POINTFUNDAMENTAL;
  CreateInitializer();
  GetFeatureVectorFromMatchFile();

  RunInitializer();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
      return RUN_ALL_TESTS();
}
