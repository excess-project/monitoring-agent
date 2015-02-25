# RELEASE NOTES



## 0.1.4 (02/25/2015)

### Features
- Added scripts for performing experiments specific to the CRESTA CEL benchmark.
- Enabled user-defined configuration files (cf. remarks).
- Removed redundant files from the project.

### Remarks
If in the user directory <pre>$HOME/.mf</pre> a INI file exists having the PBS JobID as filename, then, for this particular job, the configuration file from the user library is used instead of the default configuration file located on the EXCESS cluster at <pre>/opt/mf/<version>/mf_config.ini</pre>.



## 0.1.3 (02/17/2015)

### Features
- Introduced first draft of a user library for code instrumentation and analysis
- Refined project structure.
- Enabled setting all parameters via the configuration file (mf_config.ini)

### Remarks
None.



## 0.1.2 (12/08/2014)

### Features
- PAPI support for monitoring all processes running on CPUs (third-person).
- Fixed parser implementation.
- Parser reads values for the PAPI plugin.
- Fixed numSockets error caused by the Likwid plugin.

### Remarks
None.



## 0.1 (12/01/2014)

### Features
- Deployed on EXCESS cluster at HLRS using TORQUE as a resource manager.
- Framework runs automatically in the background during the execution of jobs if requested.
- Refinement of the project structure (bin, lib, contrib, docs, tests).
- Redefined and unified the structure of the configuration file (mf_config.ini).
- Moved functionality of the configuration parser and database communication into sub-modules (contrib/parser, contrib/publisher) in order to prepare usage as a library.
- Introduced test-driven development using CuTest.

### Remarks
You can switch on the monitoring on the EXCESS cluster on a per-node basis by creating the file <pre>start_monitoring</pre> within the directory <pre>$HOME/.mf/service/<node></pre>.