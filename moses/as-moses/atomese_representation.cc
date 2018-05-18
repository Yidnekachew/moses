/** atomese_representation.cc ---
 *
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Author: Yidnekachew Wondimu <searchyidne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "atomese_representation.h"

using namespace opencog;
using namespace std;

/**
 * Parses a header of table data to Atomese counterpart.
 * For instance
 * i1,i2,o
 * is expected to generate
 * (List (Schema "i1") (Schema "i2") (Schema "o"))
 * @param header
 * @return
 */
Handle parse_header(const string& header)
{
	HandleSeq cols;
	for (string str : combo::tokenizeRow<string>(header))
		cols.push_back(createNode(SCHEMA_NODE, str));

	return createLink(cols, LIST_LINK);
}

/**
 * Generate an Atomese counterpart for a value row.
 * 0,1,1 is expected to generate
 * (List (Node "rx") (List (Number 0) (Number 1) (Number 1)))
 * where x is the row_number
 * @param line
 * @param row_number
 * @return
 */
Handle parse_row(const string& line, const int& row_number)
{
	HandleSeq cols;
	for (string str : combo::tokenizeRow<string>(line))
		cols.push_back(createNode(NUMBER_NODE, str));

	return createLink(LIST_LINK,
	                  createNode(NODE, "r" + to_string(row_number)),
	                  createLink(cols, LIST_LINK));
}

/**
 * Generate an Atomese representation using SimilarityLink.
 *
 * A table of data like this:
 * i1,i2,o
 *  0,1,1
 *  1,0,1
 *  0,0,0
 *
 * is expected to be represented as:
 *
 * (Similarity (stv 1 1)
 * (List (Schema "i1") (Schema "i2") (Schema "o"))
 * (Set
 *   (List (Node "r1") (List (Number 0) (Number 1) (Number 1)))
 *   (List (Node "r2") (List (Number 1) (Number 0) (Number 1)))
 *   (List (Node "r3") (List (Number 0) (Number 0) (Number 0)))))
 * @param in
 * @param repr
 * @return
 */
Handle& get_atomese_representation(istream& in, Handle& repr)
{
	HandleSeq rows;
	string line;
	int i = 0;

	// TODO: Differentiate between logical and numeric dataset.
	// TODO: What if the header isn't in the first row?
	while (combo::get_data_line(in, line))
	{
		if (i == 0)
			rows.push_back(parse_header(line));
		else
			rows.push_back(parse_row(line, i));

		i++;
	}

	repr = createLink(SIMILARITY_LINK,
	                  createLink(rows, SET_LINK));
}

/**
 * Return an Atomese representation of a CSV file.
 * @param file_name
 * @return
 */
Handle load_table(const string& file_name)
{
	OC_ASSERT(!file_name.empty(), "the file name is empty");
	ifstream in(file_name.c_str());
	OC_ASSERT(in.is_open(), "Could not open %s", file_name.c_str());

	Handle res;
	get_atomese_representation(in, res);
	return res;
}

int main(int argc, char **argv)
{
	// requires a file_name parameter.
	if(argc != 2)
		exit(1);

	string file_name = string(argv[1]);

	cout << load_table(file_name)->to_string();
}
