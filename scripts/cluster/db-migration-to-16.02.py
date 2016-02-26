#!/usr/bin/env python
#  Copyright (C) 2016 University of Stuttgart
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
from __future__ import print_function

"""Data model migration tool for Elasticsearch."""

__author__      = "Dennis Hoppe"
__copyright__   = "Copyright 2016, University of Stuttgart"

import datetime
import glob
import locale
import os
import requests
import shutil
import sys
import tarfile
import time
import elasticsearch

source_directory = 'es_backups'
backup_directory = 'es_backup'
backup_repository = 'es_backup'

def print_progress(message):
    print(message, end='\r')
    sys.stdout.flush()

def extract_backup(filename):
    print_progress(' '.join([ 'extracting', filename, '...' ]))
    tar = tarfile.open(filename)
    tar.extractall()
    tar.close()

    extracted_dir = os.path.basename(filename)[:-len(".tar.gz")]
    shutil.move(extracted_dir, backup_directory)
    print('extracting', filename, '... done')

def cleanup(directory):
    print_progress(' '.join([ 'removing', directory, '...' ]))
    shutil.rmtree(backup_directory)
    print('removing', directory, '... done')

def merge_two_dicts(x, y):
    '''Given two dicts, merge them into a new dict as a shallow copy.'''
    z = x.copy()
    z.update(y)
    return z

def transform_model(index, prev):
    new_data = {}
    prev = prev['_source']

    if not prev:
        return new_data

    if 'Timestamp' in prev:
        new_data['@timestamp'] = convert_date(prev['Timestamp'])
        del prev['Timestamp']
    else:
        return {}

    if 'type' in prev:
        new_data['type'] = prev['type']
        del prev['type']
    else:
        return {}

    if 'hostname' in prev:
        new_data['host'] = prev['hostname']
        del prev['hostname']

    new_data = merge_two_dicts(new_data, prev)
    return new_data

def convert_date(previous_date):
    try:
        epoch = float(previous_date)
        date_object = datetime.datetime.fromtimestamp(epoch)
    except ValueError as te:
        try:
            locale.setlocale(locale.LC_ALL, 'de_DE.utf8')
            date_object = datetime.datetime.strptime(previous_date, '%a %d %b %Y %H:%M:%S %Z')
        except:
            previous_date = previous_date.replace('Tu ', 'Tue ')
            try:
                locale.setlocale(locale.LC_ALL, 'en_US.utf8')
                date_object = datetime.datetime.strptime(previous_date, '%a %d %b %Y %H:%M:%S %Z')
            except:
                try:
                    locale.setlocale(locale.LC_ALL, 'en_US.utf8')
                    date_object = datetime.datetime.strptime(previous_date, '%a %d %b %Y %H:%M:%S')
                except:
                    try:
                        locale.setlocale(locale.LC_ALL, 'en_US.utf8') # Thu Nov 26 16:25:49 2015
                        date_object = datetime.datetime.strptime(previous_date, '%a %b %d %H:%M:%S %Y')
                    except:
                        print('Could not convert date:', previous_date)
                        return

    # yyyy-MM-dd'T'HH:mm:ss.SSS
    string = date_object.strftime("%Y-%m-%dT%H:%M:%S.%f")
    string = string[:-3]
    return string

def load_snapshot():
    es_client = elasticsearch.Elasticsearch()
    snapshot_client = elasticsearch.client.SnapshotClient(es_client)
    indices_client = elasticsearch.client.IndicesClient(es_client)

    es_client.cluster.health(wait_for_status='yellow', request_timeout=1)

    # close indices
    try:
        indices_client.close('mf')
    except (elasticsearch.exceptions.NotFoundError):
        pass
    try:
        indices_client.close('executions')
    except (elasticsearch.exceptions.NotFoundError):
        pass

    retval = snapshot_client.get(backup_repository, 'snapshot_*')
    snapshot = retval['snapshots'][0]['snapshot']

    # load snapshot
    print_progress(' '.join([ 'loading', snapshot, '...']))
    body = {}
    body["ignore_unavailable"] = "true"
    body["include_global_state"] = "false"

    print('loading', snapshot, '... done')

    # updating user and experiment info
    body["indices"] = "executions"
    try:
        snapshot_client.restore(backup_repository, snapshot, body)
    except elasticsearch.exceptions.TransportError as te:
        print("Could not restore backup:", snapshot, ">", te.info)
        return
    try:
        indices_client.open('mf')
        time.sleep(500 / 1000.0)
    except:
        print("Could not open /mf. Abort.")
        return
    try:
        indices_client.open('executions')
        time.sleep(500 / 1000.0)
    except:
        print("Could not open /executions. Abort.")
        return

    try:
        size = es_client.count(index="executions")['count']
        executions = es_client.search(index="executions", size=size)
    except elasticsearch.exceptions.TransportError as e:
        print("Could not search index", 'executions')
        return

    id_mapper = {}
    user_mapper = {}
    task_mapper = {}
    for execution in executions['hits']['hits']:
        _id = execution['_id']
        execution = execution['_source']

        # new experiment data
        experiments = {}
        if 'host' not in execution:
            experiments['host'] = "fe.excess-project.eu"
        else:
            experiments['host'] = execution['hostname']
        if execution['Username'] == 'root': # remove 'root' experiments
            continue
        if execution['Username'] == 'hopped': # remove 'hopped' experiments
            continue
        experiments['user'] = execution['Username']

        # Do 19 Nov 2015 17:45:32 CET --> 2015-11-19T17:45:32.000
        experiments['@timestamp'] = convert_date(execution['Start_date'])
        experiments['job_id'] = execution['Name']
        experiments['application'] = execution['Description'].replace(' ', '_')

        # POST /users/goud/create { experiments }
        path = '/'.join(['http://127.0.0.1:3030/v1/mf/users', experiments['user'], 'create'])
        response = requests.post(path, data=experiments)
        id_mapper[_id.lower()] = response.content
        user_mapper[_id.lower()] = experiments['user']
        task_mapper[_id.lower()] = experiments['application']

    # go through restored indices
    indices = retval['snapshots'][0]['indices']
    for index in indices:
        if index in id_mapper:
            new_id = id_mapper[index]
            user = user_mapper[index]
            task = task_mapper[index]
        else:
            continue

        body["indices"] = index
        try:
            snapshot_client.restore(backup_repository, snapshot, body);
        except elasticsearch.exceptions.TransportError as te:
            print("Could not restore index", index, te.info)
            continue

        time.sleep(1)

        try:
            size = es_client.count(index=index)['count']
            if size >= 10000:
                max_window_size = {}
                max_window_size['index.max_result_window'] = int(size + 100)
                indices_client.put_settings(index=index, body=max_window_size)
            json = es_client.search(index=index, size=size, filter_path=['hits.hits._source'])
            for data in json['hits']['hits']:
                new_data = transform_model(index, data)
                if new_data:
                    path = '/'.join(['http://127.0.0.1:3030/v1/mf/metrics', user, new_id])
                    path = path + '?task=' + task
                    response = requests.post(path, data=new_data)
        except elasticsearch.exceptions.TransportError as te:
            print("Could not retrieve index", index, te.info)
            continue

        indices_client.delete(index=index, ignore=[400, 404])
        time.sleep(500 / 1000.0)

for filename in glob.iglob(os.path.join(source_directory, '*.tar.gz')):
    extract_backup(filename)
    load_snapshot()
    cleanup(backup_directory)
