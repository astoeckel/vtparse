#  VTParse -- An implementation of Paul Williams' DEC compatible state machine
#
#  Copyright (C) 2007  Joshua Haberman
#  Copyright (C) 2018  Andreas Stöckel
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

project('vtparse',
    ['c'],
    default_options: ['c_std=c89', 'b_ndebug=if-release'])

# Build the vtparse library
inc_vtparse = include_directories('./')
lib_vtparse = library(
    'vtparse',
    [
        'vtparse/vtparse.c',
        'vtparse/vtparse_table.c'
    ],
    include_directories: [inc_vtparse]
)

# Declare the dependency that can be exported to other meson projects
dep_vtparse = declare_dependency(
    link_with: [lib_vtparse],
    include_directories: [inc_vtparse]
)

# Build the demo
exe_vtparse_example = executable(
    'vtparse_example',
    [
        'example/vtparse_example.c',
    ],
    dependencies: [dep_vtparse]
)

# Build the unit test
dep_foxenunit = dependency(
    'libfoxenunit',
    fallback:['libfoxenunit', 'dep_foxenunit'])
exe_test_vtparse = executable(
    'test_vtparse',
    [
        'test/test_vtparse.c'
    ],
    dependencies: [dep_foxenunit, dep_vtparse]
)
test('test_vtparse', exe_test_vtparse)
