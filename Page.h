#pragma once
#ifndef PAGE_H
#define PAGE_H
class Page {
private:
	std::string name;
	int pid;
	int memPerPage;

public:
	Page(std::string name, int pid, int memPerPage) : name(name), pid(pid), memPerPage(memPerPage) {}

	int getName() const { return pid; }
	int getPid() const { return pid; }
	int getMemPerPage() const { return memPerPage; }
};

#endif