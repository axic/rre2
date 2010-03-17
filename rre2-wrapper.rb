require 'rre2'

class String
  def match(pattern)
    RRE2.new(pattern).match(self.to_s)
  end

  def gsub(pattern, replacement)
    RRE2.gsub(self.to_s, pattern, replacement)
  end

# to be implemented: gsub!, scan
end
