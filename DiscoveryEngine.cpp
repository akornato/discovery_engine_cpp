/**
 * Copyright 2015 Inferapp
 */

// DiscoveryEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "DiscoveryEngine.h"

// definitions and default initializations
// of static DiscoveryEngine containers
// as opposed to their definitions in DiscoveryEngine.h
DiscoveryRules DiscoveryEngine::discoveryRules;
unordered_multimap<int, int> DiscoveryEngine::discoveryVERs;
unordered_map<int, DiscoverySignature> DiscoveryEngine::discoverySignatures;
unordered_map<string, DiscoverySource> DiscoveryEngine::discoveryAggregateSources;
mutex DiscoveryEngine::mutexDiscoveryAggregateSources;
mutex DiscoveryEngine::mutexDiscoveryResults;
unordered_map<string, DiscoveryAggregateResult> DiscoveryEngine::discoveryAggregateResults;
mutex DiscoveryEngine::mutexDiscoveryAggregateResults;

size_t DiscoverySource::moveCtorCalls;
size_t DiscoverySource::copyCtorCalls;

int _tmain(int argc, _TCHAR* argv[])
{
	time_t start = time(0);

	try {
		filesystem::remove_all("s:\\results\\");
		filesystem::create_directory("s:\\results\\");
	}
	catch (const boost::filesystem::filesystem_error& e) {
		cout << e.what();
		std::system("pause");
		return 1;
	}

	DiscoveryEngine::loadDiscoveryRules();
	DiscoveryEngine::loadDiscoverySignatures();

	//if (argc == 1)
	//{
	DiscoveryEngine::processAllScans();

	DiscoveryEngine::saveDiscoveryAggregateResults();
	DiscoveryEngine::saveDiscoveryAggregateSources();
	//}
	//else
	//DiscoveryEngine::loadDiscoveryAggregateSources();

	// log execution time
	ofstream ofs("s:\\logs\\execution_times.txt", fstream::app | fstream::out);
	ofs << "copyCtorCalls: " << DiscoverySource::copyCtorCalls << endl;
	ofs << "moveCtorCalls: " << DiscoverySource::moveCtorCalls << endl;
	ofs << "Total (C++): " << time(0) - start << endl << endl;
	ofs.close();

	return 0;
}