require 'rre2-wrapper'

puts "This will     be replaced".gsub("will\s+be", "was")

puts "Simple text to run match on".match("text")