Gem::Specification.new do |s|
  s.name = %{rre2}
  s.version = "0.1"
  s.date = %q{2010-03-16}
  s.authors = ["Alex Beregszaszi"]
  s.email = %q{alex@siqon.com}
  s.summary = %q{RRE2 is a Ruby RE2 binding.}
  s.description = s.summary = %q{RRE2 is a Ruby RE2 binding.}
  s.extensions = [ "extconf.rb" ]
  s.files = [ "COPYING", "README", "MANIFEST", "extconf.rb", "rre2.cc"]
end
