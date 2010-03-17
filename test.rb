require 'rre2'

data = "This <should not, and <this shouldn't $ be selected, but <this should be> <or this>."
exp = /<[^$<>]*>/

test = Regexp.new(exp)
["inspect","source","to_s"].each { |x| puts "Builtin #{x}: #{test.send(x)}" }
puts "Builtin Quote: #{test.inspect}"
puts "Builtin Match: #{test.match(data)}"
puts "Builtin Match: #{test.match(data).to_a.inspect}"
puts "Builtin Escape: #{Regexp.escape(exp.source)}"

puts test.match(data).inspect

test = RRE2.new(exp.source)
puts "New Quote: #{test.inspect}"
puts "New Match: #{test.match(data)}"
puts "New Match: #{test.match(data).to_a.inspect}"
puts "New Escape: #{RRE2.escape(exp.source)}"

puts "Program size: #{test.program_size}"

puts test.match(data).inspect

#puts "This will be splitted".scan(RRE2.new("\w+"))

#puts RRE2.new("\w+").scan("This will be splitted").inspect
puts RRE2.new("[^ ]*").match("This will be splitted").inspect
