require "mkmf"

dir_config("re2")
have_library("re2")
#have_header("re2.h", 'RE2 pattern(".*");')
have_header("re2.h")
have_library("stdc++")
create_makefile("rre2")
