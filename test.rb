require 'rre2'

data = "This <should not, and <this shouldn't $ be selected, but <this should be> <or this>."
exp = /<[^$<>]*>/

test = Regexp.new(exp)
["inspect","source","to_s"].each { |x| puts "Builtin #{x}: #{test.send(x)}" }
puts "Builtin Quote: #{test.inspect}"
puts "Builtin Match: #{test.match(data)}"
puts "Builtin Match: #{test.match(data).to_a.inspect}"

puts test.match(data).inspect

test = RRE2.new(exp.source)
puts "New Quote: #{test.inspect}"
puts "New Match: #{test.match(data)}"
puts "New Match: #{test.match(data).to_a.inspect}"
