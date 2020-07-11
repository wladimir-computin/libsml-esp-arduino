// Copyright 2020 Matthias Bher
//
// This file is part of libSML.
//
// libSML is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libSML is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libSML.  If not, see <http://www.gnu.org/licenses/>.

#include <sml/sml_file.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	sml_file *file = sml_file_parse((unsigned char *)data, size);
	sml_file_free(file);
	return 0;
}

// compile with:
// clang -g -O1 -L./sml/lib -fsanitize=fuzzer test/src/fuzzer.c -lsml -luuid

// run with:
// mkdir corpus_dir
// ./a.out corpus_dir
// run until it stops with a crash file being generated
// e.g. crash-...
// debug with e.g.
// gdb --args ./a.out crash-...
