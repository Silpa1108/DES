#ifndef PATH_H
#define PATH_H

#include<string>
/*Path class to parse all the path information from system model to repesent the system model as directed graph structure later*/
class Path
{
protected:
	std::string sourceNode;
	std::string targetNode;

public:
	Path(const std::string& source, const std::string& target);
	void GetSourceTargetNodes(std::string& source, std::string& target);
	const  std::string& get_SourceNode() const { return sourceNode; }
	const  std::string& get_TargetNode() const { return targetNode; }
	~Path() {}
};



#endif /* PATH */
