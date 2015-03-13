template <typename T>
std::string NumberToString ( T Number )
{
  std::ostringstream ss;
  ss << Number;
  return ss.str();
}

template <typename T>
T StringToNumber ( std::string str )
{
  std::istringstream ss(str);
  T val;
  ss >> val;
  return val;
}
