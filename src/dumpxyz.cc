/*
    Copyright (C) 2015 Carl Hetherington <cth@carlh.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <dcp/mono_picture_mxf.h>
#include <dcp/mono_picture_frame.h>
#include <dcp/xyz_image.h>
#include <getopt.h>
#include <string>
#include <iostream>

using std::string;
using std::cerr;
using std::cout;
using boost::shared_ptr;
using boost::optional;

static void
help (string n)
{
	cerr << "Syntax: " << n << " [OPTIONS] <MXF>\n"
	     << "  -h, --help         show this help\n"
	     << "  -f, --frame        frame index to dump\n"
	     << "\n"
	     << "<MXF> is a video MXF file.\n";
}

static void
dump (dcp::MonoPictureMXF& mxf, int64_t frame)
{
	cout << "Frame " << frame << "\n";
	cout << "Size " << mxf.size().width << " " << mxf.size().height << "\n";

	shared_ptr<const dcp::XYZImage> xyz = mxf.get_frame(frame)->xyz_image ();
	int* xyz_x = xyz->data (0);
	int* xyz_y = xyz->data (1);
	int* xyz_z = xyz->data (2);

	for (int y = 0; y < mxf.size().height; ++y) {
		cout << "Row " << y << "\n";
		for (int x = 0; x < mxf.size().width; ++x) {
			cout << *xyz_x << " " << *xyz_y << " " << *xyz_z << "\n";
			++xyz_x;
			++xyz_y;
			++xyz_z;
		}
	}
}

int
main (int argc, char* argv[])
{
	optional<int> frame;
	
	int option_index = 0;
	while (true) {
		static struct option long_options[] = {
			{ "help", no_argument, 0, 'h'},
			{ "frame", required_argument, 0, 'f'},
			{ 0, 0, 0, 0 }
		};

		int c = getopt_long (argc, argv, "hf", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 'h':
			help (argv[0]);
			exit (EXIT_SUCCESS);
		case 'f':
			frame = atoi (optarg);
			break;
		}
	}

	if (optind >= argc) {
		help (argv[0]);
		exit (EXIT_FAILURE);
	}

	dcp::MonoPictureMXF mxf (argv[optind]);

	if (frame) {
		dump (mxf, frame.get ());
	} else {
		for (int64_t i = 0; i < mxf.intrinsic_duration(); i++) {
			dump (mxf, i);
		}
	}
}
