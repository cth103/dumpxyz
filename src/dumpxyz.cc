/*
    Copyright (C) 2015-2016 Carl Hetherington <cth@carlh.net>

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

#include <dcp/mono_picture_asset.h>
#include <dcp/mono_picture_asset_reader.h>
#include <dcp/mono_picture_frame.h>
#include <dcp/openjpeg_image.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cerr;
using std::cout;
using std::ostream;
using std::ofstream;
using boost::shared_ptr;
using boost::optional;

static void
help (string n)
{
	cerr << "Syntax: " << n << " [OPTIONS] <MXF>\n"
	     << "  -h, --help         show this help\n"
	     << "  -f, --frame        frame index to dump\n"
	     << "  -r, --row          row index to dump\n"
	     << "\n"
	     << "<MXF> is a video MXF file.\n";
}

static void
dump (dcp::MonoPictureAsset& mxf, ostream& s, int64_t frame, optional<int> row)
{
	s << "Frame " << frame << "\n";
	s << "Size " << mxf.size().width << " " << mxf.size().height << "\n";

	shared_ptr<dcp::MonoPictureAssetReader> reader = mxf.start_read ();

	shared_ptr<const dcp::OpenJPEGImage> xyz = reader->get_frame(frame)->xyz_image ();
	int* xyz_x = xyz->data (0);
	int* xyz_y = xyz->data (1);
	int* xyz_z = xyz->data (2);

	int const width = mxf.size().width;
	int const height = mxf.size().height;

	if (!row) {
		for (int y = 0; y < height; ++y) {
			s << "Row " << y << "\n";
			for (int x = 0; x < width; ++x) {
				s << *xyz_x << " " << *xyz_y << " " << *xyz_z << "\n";
				++xyz_x;
				++xyz_y;
				++xyz_z;
			}
		}
	} else {
		xyz_x += row.get() * width;
		xyz_y += row.get() * width;
		xyz_z += row.get() * width;
		s << "Row " << row.get() << "\n";
		for (int x = 0; x < width; ++x) {
			s << *xyz_x << " " << *xyz_y << " " << *xyz_z << "\n";
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
	optional<int> row;
	optional<boost::filesystem::path> output;

	int option_index = 0;
	while (true) {
		static struct option long_options[] = {
			{ "help", no_argument, 0, 'h'},
			{ "frame", required_argument, 0, 'f'},
			{ "row", required_argument, 0, 'r'},
			{ "output", required_argument, 0, 'o'},
			{ 0, 0, 0, 0 }
		};

		int c = getopt_long (argc, argv, "hf:r:o:", long_options, &option_index);

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
		case 'r':
			row = atoi (optarg);
			break;
		case 'o':
			output = optarg;
			break;
		}
	}

	if (optind >= argc) {
		help (argv[0]);
		exit (EXIT_FAILURE);
	}

	dcp::MonoPictureAsset mxf (argv[optind]);

	ostream* s = 0;
	bool delete_stream = false;
	if (output) {
		s = new ofstream (output.get().string().c_str());
		delete_stream = true;
	} else {
		s = &cout;
	}

	if (frame) {
		dump (mxf, *s, frame.get (), row);
	} else {
		for (int64_t i = 0; i < mxf.intrinsic_duration(); i++) {
			dump (mxf, *s, i, row);
		}
	}

	if (delete_stream) {
		delete s;
	}
}
