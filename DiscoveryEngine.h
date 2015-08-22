/**
 * Copyright 2015 Inferapp
 */

#pragma once

#include "stdafx.h"

/**
* The <code>DiscoverySource</code> class represents either addremove, file or pkginst discovery source.
* @author Inferapp
* @version 1.0
*/
struct DiscoverySource
{
	static size_t moveCtorCalls;
	static size_t copyCtorCalls;

	/** 0: file, 1: addremove, 2: pkginst */
	int sourceTypeID;

	/** File name, addremove description, or pkginst name.*/
	string sourceKeyOriginal;
	/** File name, addremove description, or pkginst name. Uppercased for key usage.*/
	string sourceKeyUpperCase;

	string sourceProductVersion;
	string sourceProductName;
	string sourceFileVersion;
	string sourceFileSize;
	string sourceFilePath;
	string sourceFileDescription;
	string sourceCompanyName;
	//string sourceInstalledLocation;
	//string sourceUninstallString;
	//string sourceOSComponent;
	string sourceScanPath;

	DiscoverySource() {}

	// constructor for addremoves
	DiscoverySource(string& sourceKeyOriginal, string& sourceProductVersion, string& sourceCompanyName) : sourceKeyOriginal(sourceKeyOriginal),
		sourceKeyUpperCase(std::move(sourceKeyOriginal)), sourceProductVersion(std::move(sourceProductVersion)), sourceCompanyName(std::move(sourceCompanyName))
	{
		sourceTypeID = 1;
		to_upper(this->sourceKeyUpperCase);
	}

	// constructor for files
	DiscoverySource(string& sourceKeyOriginal, string& sourceProductVersion, string& sourceCompanyName,
		string& sourceProductName, string& sourceFileDescription, string& sourceFileVersion, string& sourceFileSize,
		string& sourceFilePath) : sourceKeyOriginal(sourceKeyOriginal),
		sourceKeyUpperCase(std::move(sourceKeyOriginal)), sourceProductVersion(std::move(sourceProductVersion)), sourceCompanyName(std::move(sourceCompanyName)),
		sourceProductName(std::move(sourceProductName)), sourceFileDescription(std::move(sourceFileDescription)), sourceFileVersion(std::move(sourceFileVersion)),
		sourceFileSize(std::move(sourceFileSize)), /* change file size to int, get rid of move */ sourceFilePath(std::move(sourceFilePath))
	{
		sourceTypeID = 0;
		to_upper(this->sourceKeyUpperCase);
	}

	// copy constructor
	DiscoverySource(const DiscoverySource& source) : sourceTypeID(source.sourceTypeID), sourceKeyOriginal(source.sourceKeyOriginal), sourceKeyUpperCase(source.sourceKeyUpperCase),
		sourceProductVersion(source.sourceProductVersion), sourceProductName(source.sourceProductName), sourceFileVersion(source.sourceFileVersion),
		sourceFileSize(source.sourceFileSize), sourceFilePath(source.sourceFilePath), sourceFileDescription(source.sourceFileDescription),
		sourceCompanyName(source.sourceCompanyName), sourceScanPath(source.sourceScanPath)
	{
		++copyCtorCalls;
	}

	// move constructor
	DiscoverySource(DiscoverySource&& source) : sourceTypeID(source.sourceTypeID), sourceKeyOriginal(std::move(source.sourceKeyOriginal)), sourceKeyUpperCase(std::move(source.sourceKeyUpperCase)),
		sourceProductVersion(std::move(source.sourceProductVersion)), sourceProductName(std::move(source.sourceProductName)), sourceFileVersion(std::move(source.sourceFileVersion)),
		sourceFileSize(std::move(source.sourceFileSize)), sourceFilePath(std::move(source.sourceFilePath)), sourceFileDescription(std::move(source.sourceFileDescription)),
		sourceCompanyName(std::move(source.sourceCompanyName)), sourceScanPath(std::move(source.sourceScanPath))
	{
		++moveCtorCalls;
	}
};

/**
* The <code>DiscoveryRule</code> class represents either addremove, file or pkginst discovery rule.
* @author Inferapp
* @version 1.0
*/
struct DiscoveryRule
{
	int versionID;
	int buildID;

	/** 0: Autonumber.*/
	int ruleID;

	/** 0: file, 1: addremove, 2: pkginst */
	int sourceTypeID;

	/** File name, addremove description, or pkginst name. Uppercased for key usage.*/
	string ruleKeyUpperCase;
	/** File name, addremove description, or pkginst name.*/
	string ruleKeyOriginal;

	/** Simple glob style wildcard allowed, replaced with .* for regex matching.*/
	string ruleProductVersion;
	bool isRuleProductVersionRegex;

	/** Simple glob style wildcard allowed, replaced with .* for regex matching. Case insensitivity is provided by ECMAScript | icase switch in regex constructor.*/
	string ruleProductName;
	bool isRuleProductNameRegex;

	/** Simple glob style wildcard allowed, replaced with .* for regex matching.*/
	string ruleFileVersion;
	bool isRuleFileVersionRegex;

	string ruleFileSize;

	/**
	* Simple glob style wildcard allowed, replaced with .* for regex matching. Case insensitivity is provided by ECMAScript | icase switch in regex constructor.
	* Always a regex whenever nonempty so no need for a separate boolean.
	*/
	string ruleFilePath;
};

// index tags for DiscoveryRules
struct ByBuildID{};
struct BySourceTypeIDRuleKey{};
struct BySourceTypeIDRuleKeyRuleProductVersion{};

/**
* The <code>DiscoveryRules</code> container provides lookups of discovery rules by
* buildID, sourceTypeID/ruleKeyUpperCase and sourceTypeID/ruleKeyUpperCase/ruleProductVersion.
* @author Inferapp
* @version 1.0
*/
typedef multi_index_container<
	DiscoveryRule,
	indexed_by<
	hashed_non_unique<
	tag<BySourceTypeIDRuleKeyRuleProductVersion>,
	composite_key<
	DiscoveryRule,
	member<DiscoveryRule, int, &DiscoveryRule::sourceTypeID>,
	member<DiscoveryRule, string, &DiscoveryRule::ruleKeyUpperCase>,
	member<DiscoveryRule, string, &DiscoveryRule::ruleProductVersion>
	>
	>,
	hashed_non_unique<
	tag<BySourceTypeIDRuleKey>,
	composite_key<
	DiscoveryRule,
	member<DiscoveryRule, int, &DiscoveryRule::sourceTypeID>,
	member<DiscoveryRule, string, &DiscoveryRule::ruleKeyUpperCase>
	>
	>,
	hashed_non_unique<
	tag<ByBuildID>,
	member<DiscoveryRule, int, &DiscoveryRule::buildID>
	>
	>
> DiscoveryRules;

/**
* The <code>DiscoveryMatch</code> class stores a single match between a rule and a source.
* It also provides a natural order by rule.ruleID.
* This is required for use in DiscoveryResult, which has a set of DiscoveryMatches,
* and we only care for the first match against any unique DiscoveryRule,
* so this natural ordering of DiscoveryMatch by rule.ruleID effectively prevents
* adding more than one DiscoveryMatch with same DiscoveryRule.ruleID under given DiscoveryResult.
* @author Inferapp
* @version 1.0
*/
struct DiscoveryMatch
{
	DiscoveryRule* rule;
	DiscoverySource* source;
	DiscoveryMatch(DiscoveryRule* rule, DiscoverySource* source) : rule(rule), source(source) {}

	/** Natural order by rule.ruleID.*/
	bool operator<(DiscoveryMatch other) const {
		return rule->ruleID < other.rule->ruleID;
	}
};

/**
* The <code>DiscoveryResult</code> class stores a set of discovery matches for a path/versionID/buildID.
* @author Inferapp
* @version 1.0
*/
struct DiscoveryResult
{
	string path;
	int versionID;
	int buildID;
	set<DiscoveryMatch> discoveryMatches;
	DiscoveryResult(string path, int versionID, int buildID) : path(path), versionID(versionID), buildID(buildID) {}
};

/**
* The <code>DiscoveryResults</code> container is a set ordered by path/versionID/buildID,
* which provides the required iteration capability by path natural order.
* It also facilitates searches for path/versionID/buildID (unique), path/versionID (non-unique) and path (non-unique).
* @author Inferapp
* @version 1.0
*/
typedef multi_index_container<
	DiscoveryResult,
	indexed_by<
	ordered_unique<
	composite_key<
	DiscoveryResult,
	member<DiscoveryResult, string, &DiscoveryResult::path>,
	member<DiscoveryResult, int, &DiscoveryResult::versionID>,
	member<DiscoveryResult, int, &DiscoveryResult::buildID>
	>
	>
	>
> DiscoveryResults;

/**
* The <code>DiscoveryAggregateResult</code> class stores a discovery result by
* detectionPath/versionID/buildID, with total count and first better scanPath.
* @author Inferapp
* @version 1.0
*/
struct DiscoveryAggregateResult
{
	string detectionPath;
	int versionID;
	int buildID;
	int count;
	string scanPath;
	DiscoveryAggregateResult(string detectionPath, int versionID, int buildID, int count, string scanPath) : detectionPath(detectionPath), versionID(versionID), buildID(buildID), count(count), scanPath(scanPath) {}
};

/**
* The <code>DiscoverySignature</code> class stores publisher, product and version fields,
* used for verbose software discovery results.
* @author Inferapp
* @version 1.0
*/
struct DiscoverySignature
{
	int publisherID;
	string publisherName;
	string webPage;
	int productID;
	string productName;
	string productLicensable;
	string productCategory;
	int versionID;
	string uniqueVersion;
	string build;
	string major;
	string minor;
	string edition;
	string variation;
	string licenseVersion;
};

/**
* The <code>DiscoveryEngine</code> has a static container for discovery rules
* as well as static aggregates for sources and results, which are shared by all the tasks,
* which are instances of its ProcessScanTask nested static class.
* @author Inferapp
* @version 1.0
*/
struct DiscoveryEngine
{
	/**
	* discoveryRules stores discovery rules,
	* shared by all the processing threads, see the container's class definition for details.
	*/
	static DiscoveryRules discoveryRules;

	/**
	* discoveryVERs stores version exclusion rules,
	* shared by all the processing tasks.
	* The key is excludedVersionID, the value is versionID.
	* The same excludedVersionID may be excluded by many different versionIDs, hence multimap.
	*/
	static unordered_multimap<int, int> discoveryVERs;

	/**
	* discoverySignatures is a lookup for verbose software discovery results,
	* shared by all the processing tasks
	* The key is versionID.
	*/
	static unordered_map<int, DiscoverySignature> discoverySignatures;

	/**
	* discoveryAggregateSources is an aggregate of all unique sources (addremoves/files/pkginsts),
	* shared by all the processing tasks.
	* The key is concatenation of:
	* for addremoves: sourceKeyUpperCase + sourceProductVersion + sourceCompanyName
	* for files: sourceKeyUpperCase + sourceProductVersion + sourceCompanyName + sourceProductName
	* + sourceFileDescription + sourceFileVersion + sourceFileSize
	*/
	static unordered_map<string, DiscoverySource> discoveryAggregateSources;
	static mutex mutexDiscoveryAggregateSources;

	/**
	* discoveryAggregateResults is an aggregate of all discovery results, unique by detectionPath/versionID/buildID,
	* shared by all the processing tasks.
	* The key is buildID + uppercased detectionPath (no need for versionID because buildID uniquely determines versionID).
	*/
	static unordered_map<string, DiscoveryAggregateResult> discoveryAggregateResults;
	static mutex mutexDiscoveryAggregateResults;

	// for saving scan-specific results
	static mutex mutexDiscoveryResults;


	/**
	* The <code>ProcessScanTask</code> static class is nested within DiscoveryEngine
	* so that it has access to its static containers, shared by all the tasks.
	* It is instantiated for each task.
	* @author Inferapp
	* @version 1.0
	*/
	static struct ProcessScanTask
	{
		/** The scan to be processed by the task.*/
		string sourceScanPath;

		/** Input scan data, i.e. addremoves/files/pkginsts. We just need one simple iteration in any order so ArrayList is sufficient.*/
		vector<DiscoverySource> discoveryMachineSources;

		/** The container to build discovery results for the scan, see the container's class definition for details.*/
		DiscoveryResults discoveryMachineResults;

		ProcessScanTask(string sourceScanPath) : sourceScanPath(sourceScanPath) {}

		void operator () ()
		{
			loadScan();

			processScan();

			saveDiscoveryMachineResults();
		}

		void loadScan()
		{
			ifstream ifs(sourceScanPath);
			if (!ifs)
			{
				cout << "Error opening scan file" << endl;
				std::system("pause");
				return;
			}

			string line;
			int mode = -1; // 0 for files, 1 for addremoves
			while (getline(ifs, line))
			{
				if (boost::starts_with(line, "<SourceName=AddRemoves>")) {
					mode = 1;
					continue;
				}
				else if (boost::starts_with(line, "<SourceName=Files>")) {
					mode = 0;
					continue;
				}

				if (mode == 1) {
					// <Fields=DisplayName		DisplayVersion	Publisher	InstallLocation	UninstallString		SystemComponent>
					vector<string> fields;
					boost::split(fields, line, boost::is_any_of("\t"));

					if (fields.size() != 6) {
						cout << "In the scan: \n" + sourceScanPath + "\nthe following line is corrupted:" << endl;
						for (auto it = fields.begin(); it != fields.end(); it++)
							cout << (*it) << "\t";
						cout << endl;
						continue;
					}

					// see DiscoverySource constructor for addremoves
					DiscoverySource source(fields[0], fields[1], fields[2]);

					string key = source.sourceKeyUpperCase + source.sourceProductVersion + source.sourceCompanyName;

					mutex::scoped_lock lock(mutexDiscoveryAggregateSources);
					if (discoveryAggregateSources.find(key) == discoveryAggregateSources.end())
					{
						source.sourceScanPath = sourceScanPath;
						discoveryAggregateSources.insert(make_pair(key, source));
					}

					discoveryMachineSources.push_back(std::move(source));
				}
				else if (mode == 0) {
					// <Fields=FilePath	FileName	ProductVersion	CompanyName	ProductName	FileDescription	FileVersion	FileSize>
					vector<string> fields;
					boost::split(fields, line, boost::is_any_of("\t"));

					if (fields.size() != 8) {
						cout << "In the scan: \n" + sourceScanPath + "\nthe following line is corrupted:\n";
						for (auto it = fields.begin(); it != fields.end(); it++)
							cout << (*it) << "\t";
						cout << "\n";
						continue;
					}

					// see DiscoverySource constructor for files
					DiscoverySource source(fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], fields[7], fields[0]);

					string key = source.sourceKeyUpperCase + source.sourceProductVersion + source.sourceCompanyName + source.sourceProductName
						+ source.sourceFileDescription + source.sourceFileVersion + source.sourceFileSize;

					mutex::scoped_lock lock(mutexDiscoveryAggregateSources);
					if (discoveryAggregateSources.find(key) == discoveryAggregateSources.end())
					{
						source.sourceScanPath = sourceScanPath;
						discoveryAggregateSources.insert(make_pair(key, source));
					}

					discoveryMachineSources.push_back(std::move(source));
				}
			}
			ifs.close();
		}

		void processScan()
		{
			// build matches between sources and rules
			for (auto itSource = discoveryMachineSources.begin(); itSource != discoveryMachineSources.end(); itSource++)
			{
				// find all rules matching the source on sourceTypeID and sourceKeyUpperCase
				auto range = discoveryRules.get<BySourceTypeIDRuleKey>().equal_range(boost::make_tuple(itSource->sourceTypeID, itSource->sourceKeyUpperCase));
				for (auto itRule = range.first; itRule != range.second; itRule++)
				{
					// eliminate rules whose remaining non-empty attributes do not match the source
					if (!itRule->ruleProductVersion.empty())
						if (!itRule->isRuleProductVersionRegex	&& itSource->sourceProductVersion != itRule->ruleProductVersion)
							continue;
						else if (itRule->isRuleProductVersionRegex && !std::regex_match(itSource->sourceProductVersion, std::regex(itRule->ruleProductVersion)))
							continue;

					if (!itRule->ruleProductName.empty())
						if (!itRule->isRuleProductNameRegex && !boost::iequals(itSource->sourceProductName, itRule->ruleProductName))
							continue;
						else if (itRule->isRuleProductNameRegex && !std::regex_match(itSource->sourceProductName, std::regex(itRule->ruleProductName, ECMAScript | icase)))
							continue;

					if (!itRule->ruleFileVersion.empty())
						if (!itRule->isRuleFileVersionRegex && itSource->sourceFileVersion != itRule->ruleFileVersion)
							continue;
						else if (itRule->isRuleFileVersionRegex && !std::regex_match(itSource->sourceFileVersion, std::regex(itRule->ruleFileVersion)))
							continue;

					if (!itRule->ruleFileSize.empty() && itSource->sourceFileSize != itRule->ruleFileSize)
						continue;

					if (!itRule->ruleFilePath.empty() && !std::regex_match(itSource->sourceFilePath, std::regex(itRule->ruleFilePath, ECMAScript | icase)))
						continue;

					// if it gets to this point then the rule matches the source on all attributes
					// and we will add a new DiscoveryMatch to discoveryMachineResults
					// for an existing DiscoveryResult with current sourceFilePath, versionID and buildID
					// or to a such newly added DiscoveryResult if it does not yet exist
					auto itResult = discoveryMachineResults.find(boost::make_tuple(itSource->sourceFilePath, itRule->versionID, itRule->buildID));
					if (itResult == discoveryMachineResults.end())
					{
						// if it does not exist yet, then create a new DiscoveryResult
						DiscoveryResult result(itSource->sourceFilePath, itRule->versionID, itRule->buildID);
						// then add the new DiscoveryMatch to it
						result.discoveryMatches.insert(DiscoveryMatch(const_cast<DiscoveryRule*>(&(*itRule)), &(*itSource)));
						// and add the new DiscoveryResult to discoveryMachineResults
						discoveryMachineResults.insert(result);
					}
					else
					{
						// if it does exist, then just add a new DiscoveryMatch to the existing DiscoveryResult
						// while natural ordering of DiscoveryMatch by ruleID in the discoveryMatches set prevents
						// adding more than one DiscoveryMatch with same DiscoveryRule.ruleID under given DiscoveryResult
						set<DiscoveryMatch>* discoveryMatches = const_cast<set<DiscoveryMatch>*>(&(itResult->discoveryMatches));
						discoveryMatches->insert(DiscoveryMatch(const_cast<DiscoveryRule*>(&(*itRule)), &(*itSource)));
					}
				}
			}

			// discovery match multiplication for path based results
			// which allows to combine non-file and file based detection on concrete paths
			// and also multiple files living in the same subtree to trigger the same buildID
			// note we start with equal_range("").second which is the first path based result
			for (auto itPathResult = discoveryMachineResults.equal_range("").second; itPathResult != discoveryMachineResults.end(); itPathResult++)
			{
				// for each path based match, add all matches of the non-path detection result with matching buildID
				// which allows to combine non-file and file based detection on concrete paths
				auto itNonPathResult = discoveryMachineResults.find(boost::make_tuple("", itPathResult->versionID, itPathResult->buildID));
				if (itNonPathResult != discoveryMachineResults.end())
				{
					set<DiscoveryMatch>* discoveryMatches = const_cast<set<DiscoveryMatch>*>(&(itPathResult->discoveryMatches));
					discoveryMatches->insert(itNonPathResult->discoveryMatches.begin(), itNonPathResult->discoveryMatches.end());
				}

				// for each path based match we will add subpath matches provided their buildIDs match
				// which allows multiple files living in the same subtree to trigger the same buildID
				auto itSubPathResult = itPathResult;
				for (itSubPathResult++; itSubPathResult != discoveryMachineResults.end(); itSubPathResult++)
				{
					// find first different path
					if (itSubPathResult->path == itPathResult->path)
						continue;
					// if it is different, then check if it is in fact a subpath of path
					else if (itSubPathResult->path.find(itPathResult->path) == 0)
					{
						// if yes, check if the buildIDs match
						if (itSubPathResult->buildID == itPathResult->buildID)
						{
							// if yes, add all the subpath matches to those in the path
							set<DiscoveryMatch>* discoveryMatches = const_cast<set<DiscoveryMatch>*>(&(itPathResult->discoveryMatches));
							discoveryMatches->insert(itSubPathResult->discoveryMatches.begin(), itSubPathResult->discoveryMatches.end());
						}
					}
					// discoveryMachineResults is sorted by path first so if it is diferent and not a subpath of path then that's it
					else
						break;
				}
			}

			// prune the discovery results down to those whose matched rule count for given buildID equals discovery rule count for this buildID
			for (auto itResult = discoveryMachineResults.begin(); itResult != discoveryMachineResults.end();)
				if (itResult->discoveryMatches.size() != discoveryRules.get<ByBuildID>().count(itResult->buildID))
					itResult = discoveryMachineResults.erase(itResult);
				else
					itResult++;

			// apply version exclusion rules, erase excluded versions
			for (auto itResult = discoveryMachineResults.begin(); itResult != discoveryMachineResults.end();)
				if (isVersionExcluded(itResult->path, itResult->versionID))
					itResult = discoveryMachineResults.erase(itResult);
				else
					itResult++;

			// add to global discovered aggregate results
			mutexDiscoveryAggregateResults.lock();
			for (auto itResult = discoveryMachineResults.begin(); itResult != discoveryMachineResults.end(); itResult++)
			{
				string key = to_string(itResult->buildID) + to_upper_copy(itResult->path);
				auto it = discoveryAggregateResults.find(key);
				if (it != discoveryAggregateResults.end())
					it->second.count++;
				else
					discoveryAggregateResults.insert(make_pair(key, DiscoveryAggregateResult(itResult->path, itResult->versionID, itResult->buildID, 1, sourceScanPath)));
			}
			mutexDiscoveryAggregateResults.unlock();
		}

		// recursive, because a version may be excluded via a chain of version exclusion rules
		bool isVersionExcluded(const string& path, int oldVersionID)
		{
			bool returnValue = false;
			auto range = discoveryVERs.equal_range(oldVersionID);
			for (auto itVER = range.first; itVER != range.second; itVER++)
			{
				if (discoveryMachineResults.find(boost::make_tuple(path, itVER->second)) != discoveryMachineResults.end())
				{
					returnValue = true;
					break;
				}
				else
					returnValue = isVersionExcluded(path, itVER->second);
			}
			return returnValue;
		}

		void saveDiscoveryMachineResults()
		{
			mutex::scoped_lock lock(mutexDiscoveryResults);

			ofstream ofsResults("s:\\results\\results.txt", fstream::app | fstream::out);
			ofstream ofsResultsVerboseAddremoves("s:\\results\\results_verbose_addremoves.txt", fstream::app | fstream::out);
			ofstream ofsResultsVerboseFiles("s:\\results\\results_verbose_files.txt", fstream::app | fstream::out);

			for (auto itResult = discoveryMachineResults.begin(); itResult != discoveryMachineResults.end(); itResult++)
			{
				ofsResults << itResult->versionID << "\t" << itResult->buildID << "\t" << itResult->path << "\t" << sourceScanPath << endl;

				auto itSignature = discoverySignatures.find(itResult->versionID);
				if (itSignature != discoverySignatures.end())
					for (auto itMatch = itResult->discoveryMatches.begin(); itMatch != itResult->discoveryMatches.end(); itMatch++)
					{
						if (itMatch->rule->sourceTypeID == 0)
							ofsResultsVerboseFiles << sourceScanPath << "\t"
							<< itSignature->second.publisherID << "\t" << itSignature->second.publisherName << "\t" << itSignature->second.webPage << "\t"
							<< itSignature->second.productID << "\t" << itSignature->second.productName << "\t" << itSignature->second.productLicensable << "\t"
							<< itSignature->second.productCategory << "\t" << itSignature->second.versionID << "\t" << itSignature->second.uniqueVersion << "\t"
							<< itSignature->second.build << "\t" << itSignature->second.major << "\t" << itSignature->second.minor << "\t"
							<< itSignature->second.edition << "\t" << itSignature->second.variation << "\t" << itSignature->second.licenseVersion << "\t"
							<< "file" << "\t" << itMatch->source->sourceCompanyName << "\t"
							<< itMatch->source->sourceKeyOriginal << "\t" << itMatch->source->sourceFileDescription << "\t" << itMatch->source->sourceProductName << "\t"
							<< itMatch->source->sourceProductVersion << endl;
						else if (itMatch->rule->sourceTypeID == 1)
							ofsResultsVerboseAddremoves << sourceScanPath << "\t"
							<< itSignature->second.publisherID << "\t" << itSignature->second.publisherName << "\t" << itSignature->second.webPage << "\t"
							<< itSignature->second.productID << "\t" << itSignature->second.productName << "\t" << itSignature->second.productLicensable << "\t"
							<< itSignature->second.productCategory << "\t" << itSignature->second.versionID << "\t" << itSignature->second.uniqueVersion << "\t"
							<< itSignature->second.build << "\t" << itSignature->second.major << "\t" << itSignature->second.minor << "\t"
							<< itSignature->second.edition << "\t" << itSignature->second.variation << "\t" << itSignature->second.licenseVersion << "\t"
							<< "addremove" << "\t" << itMatch->source->sourceCompanyName << "\t" << itMatch->source->sourceKeyOriginal << "\t"
							<< itMatch->source->sourceProductVersion << endl;
					}
				else
				{
					cout << "The signature for versionID: " << itResult->versionID << " is missing.";
					std::system("pause");
				}
			}
		}
	};
	// end of ProcessScanTask class

	static void processAllScans()
	{
		time_t start = time(0);

		// write headers
		ofstream ofsResultsVerboseAddremoves("s:\\results\\results_verbose_addremoves.txt", fstream::out);
		ofsResultsVerboseAddremoves << "SourceScanPath" << "\t"
			<< "PublisherID" << "\t" << "PublisherName" << "\t" << "WebPage" << "\t"
			<< "ProductID" << "\t" << "ProductName" << "\t" << "Licensable" << "\t"
			<< "ProductCategory" << "\t" << "VersionID" << "\t" << "UniqueVersion" << "\t"
			<< "Build" << "\t" << "Major" << "\t" << "Minor" << "\t"
			<< "Edition" << "\t" << "Variation" << "\t" << "LicenseVersion" << "\t"
			<< "SourceType" << "\t" << "\t" << "SourceManufacturer" << "\t" << "SourceSoftwareName" << "\t"
			<< "SourceSoftwareVersion" << endl;
		ofsResultsVerboseAddremoves.close();

		ofstream ofsResultsVerboseFiles("s:\\results\\results_verbose_files.txt", fstream::out);
		ofsResultsVerboseFiles << "SourceScanPath" << "\t"
			<< "PublisherID" << "\t" << "PublisherName" << "\t" << "WebPage" << "\t"
			<< "ProductID" << "\t" << "ProductName" << "\t" << "Licensable" << "\t"
			<< "ProductCategory" << "\t" << "VersionID" << "\t" << "UniqueVersion" << "\t"
			<< "Build" << "\t" << "Major" << "\t" << "Minor" << "\t"
			<< "Edition" << "\t" << "Variation" << "\t" << "LicenseVersion" << "\t"
			<< "SourceType" << "\t" << "\t" << "SourceManufacturer" << "\t" << "SourceFileName" << "\t"
			<< "SourceFileDescription" << "\t" << "SourceProductName" << "\t" << "SourceProductVersion" << endl;
		ofsResultsVerboseFiles.close();

		int noOfWorkerThreads = thread::hardware_concurrency();
		cout << "Detected " << noOfWorkerThreads << " processors!" << endl;

		boost::asio::io_service ioService;
		asio::io_service::work work(ioService);
		boost::thread_group threadGroup;

		// start thread pool
		for (int i = 0; i < (noOfWorkerThreads > 1 ? noOfWorkerThreads / 2 : 1); ++i)
			threadGroup.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
		cout << "Processing scans with " << (noOfWorkerThreads > 1 ? noOfWorkerThreads / 2 : 1) << " worker threads!" << endl;

		// submit tasks to the thread pool
		try {
			for (filesystem::recursive_directory_iterator it("s:\\scans\\"); it != filesystem::recursive_directory_iterator(); it++)
				if (is_regular_file(*it) && it->path().extension() == ".scan")
					ioService.post(ProcessScanTask(it->path().string()));
		}
		catch (boost::filesystem::filesystem_error &ex){ std::cout << ex.what() << "\n"; }

		// wait for the tasks to finish
		ioService.stop();
		threadGroup.join_all();

		// log execution time
		ofstream ofs("s:\\logs\\execution_times.txt", fstream::app | fstream::out);
		ofs << "processAllScans (C++): " << time(0) - start << endl;
		ofs.close();
	}

	static void loadDiscoveryRules()
	{
		// getline only assigns strings so we need this tmp before we convert to int
		string tmp;

		// load discovery rules
		ifstream ifs("s:\\library\\DiscoveryRules.txt");
		if (!ifs)
		{
			cout << "Error opening DiscoveryRules.txt" << endl;
			std::system("pause");
			return;
		}
		int ruleID = 1;
		while (getline(ifs, tmp, '\t'))
		{
			DiscoveryRule rule;

			rule.ruleID = ruleID++;

			rule.versionID = stol(tmp);
			getline(ifs, tmp, '\t');
			rule.buildID = stol(tmp);
			getline(ifs, tmp, '\t');
			rule.sourceTypeID = stol(tmp);

			// uppercased for key usage
			getline(ifs, rule.ruleKeyUpperCase, '\t');
			rule.ruleKeyOriginal = rule.ruleKeyUpperCase;
			to_upper(rule.ruleKeyUpperCase);

			// simple glob style wildcard allowed, replace with .* for regex matching
			getline(ifs, rule.ruleProductVersion, '\t');
			if (rule.ruleProductVersion.find("*") != string::npos) {
				replaceStringInPlace(rule.ruleProductVersion, "*", ".*");
				rule.isRuleProductVersionRegex = true;
			}
			else
				rule.isRuleProductVersionRegex = false;

			// simple glob style wildcard allowed, replace with .* for regex matching
			// Case insensitivity is provided by ECMAScript | icase switch in regex constructor.
			getline(ifs, rule.ruleProductName, '\t');
			if (rule.ruleProductName.find("*") != string::npos) {
				replaceStringInPlace(rule.ruleProductName, "*", ".*");
				rule.isRuleProductNameRegex = true;
			}
			else
				rule.isRuleProductNameRegex = false;

			// simple glob style wildcard allowed, replace with .* for regex matching
			getline(ifs, rule.ruleFileVersion, '\t');
			if (rule.ruleFileVersion.find("*") != string::npos) {
				replaceStringInPlace(rule.ruleFileVersion, "*", ".*");
				rule.isRuleFileVersionRegex = true;
			}
			else
				rule.isRuleFileVersionRegex = false;

			getline(ifs, rule.ruleFileSize, '\t');

			// simple glob style wildcard allowed, replace with .* for regex matching
			// Case insensitivity is provided by ECMAScript | icase switch in regex constructor.
			getline(ifs, rule.ruleFilePath);
			if (rule.ruleFilePath.find("*") != string::npos) {
				replaceStringInPlace(rule.ruleFilePath, "*", ".*");
				// rule file path is always a regex whenever present so no need to set a separate boolean as in the previous ones
			}

			discoveryRules.insert(rule);
		}
		ifs.close();

		// load discovery version exclusion rules
		ifs.open("s:\\library\\DiscoveryVERs.txt");
		if (!ifs)
		{
			cout << "Error opening DiscoveryVERs.txt" << endl;
			std::system("pause");
			return;
		}
		while (getline(ifs, tmp, '\t'))
		{
			int excludedVersionID = stol(tmp);
			getline(ifs, tmp, '\t');
			int versionID = stol(tmp);
			getline(ifs, tmp);

			discoveryVERs.insert(make_pair(excludedVersionID, versionID));
		}
	}

	static void loadDiscoverySignatures()
	{
		ifstream ifs("s:\\library\\DiscoverySignatures.txt");
		if (!ifs)
		{
			cout << "Error opening s:\\library\\DiscoverySignatures.txt file" << endl;
			std::system("pause");
			return;
		}

		string line;
		while (getline(ifs, line))
		{
			vector<string> fields;
			boost::split(fields, line, boost::is_any_of("\t"));

			DiscoverySignature signature;
			signature.publisherID = stol(fields[0]);
			signature.publisherName = fields[1];
			signature.webPage = fields[2];
			signature.productID = stol(fields[3]);
			signature.productName = fields[4];
			signature.productLicensable = fields[5];
			signature.productCategory = fields[6];
			signature.versionID = stol(fields[7]);
			signature.uniqueVersion = fields[8];
			signature.build = fields[9];
			signature.major = fields[10];
			signature.minor = fields[11];
			signature.edition = fields[12];
			signature.variation = fields[13];
			signature.licenseVersion = fields[14];

			discoverySignatures.insert(make_pair(signature.versionID, signature));
		}
	}

	static void saveDiscoveryAggregateResults()
	{
		ofstream ofs("s:\\results\\results_aggregate.txt", fstream::app | fstream::out);
		for (auto it = discoveryAggregateResults.begin(); it != discoveryAggregateResults.end(); it++)
			ofs << it->second.versionID << "\t" << it->second.buildID << "\t" << it->second.detectionPath << "\t" << it->second.count << "\t" << it->second.scanPath << endl;
	}

	static void saveDiscoveryAggregateSources()
	{
		ofstream ofsAggregateAddremoves("s:\\results\\aggregate_addremoves.txt", fstream::app | fstream::out);
		ofstream ofsAggregateAddremovesUnused("s:\\results\\aggregate_addremoves_unused.txt", fstream::app | fstream::out);
		ofstream ofsAggregateFiles("s:\\results\\aggregate_files.txt", fstream::app | fstream::out);
		ofstream ofsAggregateFilesUnused("s:\\results\\aggregate_files_unused.txt", fstream::app | fstream::out);

		for (auto it = discoveryAggregateSources.begin(); it != discoveryAggregateSources.end(); it++)
		{
			if (it->second.sourceTypeID == 0)
			{
				ofsAggregateFiles << it->second.sourceKeyOriginal << "\t" << it->second.sourceProductVersion << "\t" << it->second.sourceCompanyName << "\t"
					<< it->second.sourceProductName << "\t" << it->second.sourceFileDescription << "\t" << it->second.sourceFileVersion << "\t" << it->second.sourceFileSize << "\t"
					<< it->second.sourceFilePath << "\t" << it->second.sourceScanPath << endl;

				auto match = discoveryRules.get<BySourceTypeIDRuleKeyRuleProductVersion>().find(boost::make_tuple(0, it->second.sourceKeyUpperCase, it->second.sourceProductVersion));
				if (match == discoveryRules.get<BySourceTypeIDRuleKeyRuleProductVersion>().end())
				{
					ofsAggregateFilesUnused << it->second.sourceKeyOriginal << "\t" << it->second.sourceProductVersion << "\t" << it->second.sourceCompanyName << "\t"
						<< it->second.sourceProductName << "\t" << it->second.sourceFileDescription << "\t" << it->second.sourceFileVersion << "\t" << it->second.sourceFileSize << "\t"
						<< it->second.sourceFilePath << "\t" << it->second.sourceScanPath << endl;
				}
			}
			else if (it->second.sourceTypeID == 1)
			{
				ofsAggregateAddremoves << it->second.sourceKeyOriginal << "\t" << it->second.sourceProductVersion << "\t" << it->second.sourceCompanyName << "\t"
					//<< it->second.sourceInstalledLocation << "\t" << it->second.sourceUninstallString << "\t" << it->second.sourceOSComponent << "\t"
					<< it->second.sourceScanPath << endl;

				auto match = discoveryRules.get<BySourceTypeIDRuleKeyRuleProductVersion>().find(boost::make_tuple(1, it->second.sourceKeyUpperCase, it->second.sourceProductVersion));
				if (match == discoveryRules.get<BySourceTypeIDRuleKeyRuleProductVersion>().end())
				{
					ofsAggregateAddremovesUnused << it->second.sourceKeyOriginal << "\t" << it->second.sourceProductVersion << "\t" << it->second.sourceCompanyName << "\t"
						//<< it->second.sourceInstalledLocation << "\t" << it->second.sourceUninstallString << "\t" << it->second.sourceOSComponent << "\t" 
						<< it->second.sourceScanPath << endl;
				}
			}
		}
	}

	static void loadDiscoveryAggregateSources()
	{
		// load aggregate addremoves
		ifstream ifs_ma("s:\\results\\aggregate_addremoves.txt");
		if (!ifs_ma)
		{
			cout << "Error opening s:\\results\\aggregate_addremoves.txt file" << endl;
			std::system("pause");
			return;
		}

		// <Fields=DisplayName		ProductVersion	Publisher	ScanPath>
		string line;
		while (getline(ifs_ma, line))
		{
			vector<string> fields;
			boost::split(fields, line, boost::is_any_of("\t"));

			DiscoverySource source;
			source.sourceTypeID = 1;
			source.sourceKeyUpperCase = fields[0];
			to_upper(source.sourceKeyUpperCase);
			source.sourceKeyOriginal = fields[0];
			source.sourceProductVersion = fields[1];
			source.sourceCompanyName = fields[2];
			source.sourceScanPath = fields[3];

			discoveryAggregateSources.insert(make_pair(to_upper_copy(source.sourceKeyUpperCase + source.sourceProductVersion + source.sourceCompanyName), source));
		}

		ifs_ma.close();

		// load aggregate files
		ifstream ifs_mf("s:\\results\\aggregate_files.txt");
		if (!ifs_mf)
		{
			cout << "Error opening s:\\results\\aggregate_files.txt file" << endl;
			std::system("pause");
			return;
		}

		// <Fields=FileName	ProductVersion	CompanyName	ProductName	FileDescription	FileVersion	FileSize	FilePath	ScanPath>
		while (getline(ifs_mf, line))
		{
			vector<string> fields;
			boost::split(fields, line, boost::is_any_of("\t"));

			DiscoverySource source;
			source.sourceTypeID = 0;
			source.sourceKeyUpperCase = fields[0];
			to_upper(source.sourceKeyUpperCase);
			source.sourceKeyOriginal = fields[0];
			source.sourceProductVersion = fields[1];
			source.sourceCompanyName = fields[2];
			source.sourceProductName = fields[3];
			source.sourceFileDescription = fields[4];
			source.sourceFileVersion = fields[5];
			source.sourceFileSize = fields[6];
			source.sourceFilePath = fields[7];
			source.sourceScanPath = fields[8];

			discoveryAggregateSources.insert(make_pair(to_upper_copy(source.sourceKeyUpperCase + source.sourceProductVersion + source.sourceCompanyName + source.sourceProductName + source.sourceFileDescription + source.sourceFileVersion + source.sourceFileSize), source));
		}

		ifs_mf.close();
	}

	static void emptyDiscoveryEngineGlobalContainers()
	{
		discoveryRules.clear();
		discoveryVERs.clear();
		discoverySignatures.clear();
		discoveryAggregateSources.clear();
		discoveryAggregateResults.clear();

	}

	static void replaceStringInPlace(string& subject, const string& search, const string& replace)
	{
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != std::string::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}
};