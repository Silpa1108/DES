#include"path.h"
Path::Path(const std::string& source, const std::string& target)
{
	this->sourceNode = source;
	this->targetNode = target;
}
void Path::GetSourceTargetNodes(std::string& source, std::string& target)
{
	source = sourceNode;
	target = targetNode;
}