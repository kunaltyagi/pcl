/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2011, Willow Garage, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 *
 */

#include <pcl/PCLPointCloud2.h>
#include <pcl/console/parse.h>
#include <pcl/console/print.h>
#include <pcl/console/time.h>
#include <pcl/features/boundary.h>
#include <pcl/io/pcd_io.h>

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

int default_k = 0;
double default_radius = 0.0;
double default_angle = M_PI / 2.0;

Eigen::Vector4f translation;
Eigen::Quaternionf orientation;

void
printHelp (int, char **argv)
{
  print_error ("Syntax is: %s input.pcd output.pcd <options>\n", argv[0]);
  print_info ("  where options are:\n");
  print_info ("                     -radius X = use a radius of Xm around each point "
              "to determine the neighborhood (default: ");
  print_value ("%f", default_radius);
  print_info (")\n");
  print_info ("                     -k X      = use a fixed number of X-nearest "
              "neighbors around each point (default: ");
  print_value ("%d", default_k);
  print_info (")\n");
  print_info ("                     -thresh X = the decision boundary (angle "
              "threshold) that marks points as boundary or regular (default: ");
  print_value ("%f", default_angle);
  print_info (")\n");
}

bool
loadCloud (const std::string &filename, pcl::PCLPointCloud2 &cloud)
{
  TicToc tt;
  print_highlight ("Loading ");
  print_value ("%s ", filename.c_str ());

  tt.tic ();
  if (loadPCDFile (filename, cloud, translation, orientation) < 0)
    return (false);
  print_info ("[done, ");
  print_value ("%g", tt.toc ());
  print_info (" ms : ");
  print_value ("%d", cloud.width * cloud.height);
  print_info (" points]\n");
  print_info ("Available dimensions: ");
  print_value ("%s\n", pcl::getFieldsList (cloud).c_str ());

  // Check if the dataset has normals
  if (pcl::getFieldIndex (cloud, "normal_x") == -1) {
    print_error ("The input dataset does not contain normal information!\n");
    return (false);
  }
  return (true);
}

void
compute (const pcl::PCLPointCloud2::ConstPtr &input, pcl::PCLPointCloud2 &output, int k,
         double radius, double angle)
{
  // Convert data to PointCloud<T>
  PointCloud<PointNormal>::Ptr xyznormals (new PointCloud<PointNormal>);
  fromPCLPointCloud2 (*input, *xyznormals);

  // Estimate
  TicToc tt;
  tt.tic ();

  print_highlight (stderr, "Computing ");

  BoundaryEstimation<pcl::PointNormal, pcl::PointNormal, pcl::Boundary> ne;
  ne.setInputCloud (xyznormals);
  ne.setInputNormals (xyznormals);
  // ne.setSearchMethod (pcl::KdTreeFLANN<pcl::PointNormal>::Ptr (new
  // pcl::KdTreeFLANN<pcl::PointNormal>));
  ne.setKSearch (k);
  ne.setAngleThreshold (static_cast<float> (angle));
  ne.setRadiusSearch (radius);

  PointCloud<Boundary> boundaries;
  ne.compute (boundaries);

  print_info ("[done, ");
  print_value ("%g", tt.toc ());
  print_info (" ms : ");
  print_value ("%d", boundaries.width * boundaries.height);
  print_info (" points]\n");

  // Convert data back
  pcl::PCLPointCloud2 output_boundaries;
  toPCLPointCloud2 (boundaries, output_boundaries);
  concatenateFields (*input, output_boundaries, output);
}

void
saveCloud (const std::string &filename, const pcl::PCLPointCloud2 &output)
{
  TicToc tt;
  tt.tic ();

  print_highlight ("Saving ");
  print_value ("%s ", filename.c_str ());

  pcl::io::savePCDFile (filename, output, translation, orientation, false);

  print_info ("[done, ");
  print_value ("%g", tt.toc ());
  print_info (" ms : ");
  print_value ("%d", output.width * output.height);
  print_info (" points]\n");
}

/* ---[ */
int
main (int argc, char **argv)
{
  print_info ("Estimate boundary points using pcl::BoundaryEstimation. For more "
              "information, use: %s -h\n",
              argv[0]);
  bool help = false;
  parse_argument (argc, argv, "-h", help);
  if (argc < 3 || help) {
    printHelp (argc, argv);
    return (-1);
  }

  // Parse the command line arguments for .pcd files
  std::vector<int> p_file_indices;
  p_file_indices = parse_file_extension_argument (argc, argv, ".pcd");
  if (p_file_indices.size () != 2) {
    print_error ("Need one input PCD file and one output PCD file to continue.\n");
    return (-1);
  }

  // Command line parsing
  int k = default_k;
  double radius = default_radius;
  double angle = default_angle;
  parse_argument (argc, argv, "-k", k);
  parse_argument (argc, argv, "-radius", radius);
  parse_argument (argc, argv, "-thresh", angle);

  // Load the first file
  pcl::PCLPointCloud2::Ptr cloud (new pcl::PCLPointCloud2);
  if (!loadCloud (argv[p_file_indices[0]], *cloud))
    return (-1);

  // Perform the feature estimation
  pcl::PCLPointCloud2 output;
  compute (cloud, output, k, radius, angle);

  // Save into the second file
  saveCloud (argv[p_file_indices[1]], output);
}
