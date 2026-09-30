namespace nl
{

template<class T>
GenericFlags<T>::GenericFlags(string value_):
	value(T::str2flags(value_.c_str()))
{}

template<class T>
GenericFlags<T>::GenericFlags(std::initializer_list<GenericFlags<T>> list):
	value(0)
{
	for (const auto & f : list)
		value |= f;
}

template<class T>
GenericFlags<T>::operator string() const
{
	char buf[256];
	T::flags2str(value, buf, sizeof(buf));
	return buf;
}

template<class T>
GenericFlags<T>::operator vector<unsigned int>() const
{
	vector<unsigned int> result;
	for (unsigned int f = 1; f <= value; f <<= 1)
		if (value & f)
			result.push_back(f);
	return result;
}

template<class T>
GenericFlags<T>::operator vector<string>() const
{
	char buf[256];
	vector<string> result;

	for (unsigned int f = 1; f <= value; f <<= 1) {
		if (value & f) {
			T::flags2str(f, buf, sizeof(buf));
			result.push_back(buf);
		}
	}
	return result;
}

}
