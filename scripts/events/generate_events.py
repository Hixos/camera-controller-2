import argparse
import re
import datetime
import os
import subprocess

from string import Template
from os import path
from typing import List


def readLines(file: str):
    with open(file, 'r') as f:
        return [line.rstrip('\r\n ').lstrip('\r\n ') for line in f.readlines()]


def has_duplicates(lst):
    if len(lst) != len(set(lst)):
        return True
    return False


def checkEvents(event_list: List[str]):
    # Event names start with EV_ and contains only uppercase letters or underscores
    re_event = re.compile(r"(?P<ev>^EV(_[A-Z\d]+)+$)")

    success = True
    # Only return lines with valid events, remove additional data
    for ev in event_list:
        m = re_event.fullmatch(ev)
        if m is None:
            print("Error: '{}' is not a valid event name".format(ev))
            success = False

    if has_duplicates(event_list):
        success = False
        print("Error: Duplicate events found.")

    return success


def checkTopics(topic_list: List[str]):
    # Event names start with EV_ and contains only uppercase letters or underscores
    re_event = re.compile(r"(?P<ev>^TOPIC(_[A-Z\d]+)+$)")

    success = True

    # Only return lines with valid events, remove additional data
    for topic in topic_list:
        m = re_event.fullmatch(topic)
        if m is None:
            print("Error: '{}' is not a valid topic name".format(topic))
            success = False

    if has_duplicates(topic_list):
        success = False
        print("Error: Duplicate topics found.")

    return success


parser = argparse.ArgumentParser(description='Generate events and topics headers from event and topic list')

parser.add_argument('event_file', type=str, help='File containing event names, 1 per line.')
parser.add_argument('topic_file', type=str, help='File containing topic names, 1 per line.')


parser.add_argument('output_folder', type=str, help='Folder to put generated files.')

args = parser.parse_args()


if not path.exists(args.event_file) or not path.exists(args.topic_file):
    print("Error reading provided input files. Terminating unsuccessfully.")
    exit()

events = readLines(args.event_file)
topics = readLines(args.topic_file)

if not checkEvents(events) or not checkTopics(topics):
    print("Terminating unsuccessfully.")
    exit()

date = datetime.datetime.now()

event_enum_str = ""
event_map_str = ""

for e in events:
    endl = ",\n" if e != events[-1] else ""
    event_enum_str += e + (" = EV_FIRST_SIGNAL" if e == events[0] else "") + endl
    event_map_str += "        {{ {event}, {string} }}{endl}".format(
        event=e, string='"' + e + '"', endl=endl
    )

topic_enum_str = ""
topic_map_str = ""

for t in topics:
    endl = ",\n" if t != topics[-1] else ""
    topic_enum_str += "    " + t + endl
    topic_map_str += "        {{ {topics}, {string} }}{endl}".format(
        topics=t, string='"' + t + '"', endl=endl
    )

with open(path.join("template", "CCEvents.h.template"), "r") as template_file:
    header_template = Template(template_file.read())

with open(path.join("template", "CCEvents.cpp.template"), "r") as template_file:
    source_template = Template(template_file.read())

header_text = header_template.substitute(date=date, events=event_enum_str, topics=topic_enum_str)
source_text = source_template.substitute(date=date, event_map=event_map_str, topic_map=topic_map_str)

try:
    os.mkdir(args.output_folder)
except:
    pass

header_file = path.join(args.output_folder, "CCEvents.h")
source_file = path.join(args.output_folder, "CCEvents.cpp")

with open(header_file, "w") as outfile:
    outfile.write(header_text)

with open(source_file, "w") as outfile:
    outfile.write(source_text)

subprocess.run(["clang-format", "-i", "-style=file", header_file, source_file])

print("All files generated successfully!")
