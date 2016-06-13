#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

java -Xmx1024m -classpath ${DIR}/build/jar/mf_extract.jar:${DIR}/lib/JSON/json.jar eu.excess.mf_extract.cli.mf_extract $@
