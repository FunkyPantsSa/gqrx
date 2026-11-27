#!/usr/bin/env python3
"""Check the Chinese Qt catalog and source coverage using only Python's stdlib."""

import argparse
from collections import Counter
from html.parser import HTMLParser
from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET


ROOT = Path(__file__).resolve().parents[1]
PLACEHOLDER = re.compile(r"%(?:L?[1-9][0-9]*|L?n)")
RICH_TEXT = re.compile(r"</?[a-zA-Z][a-zA-Z0-9]*(?:\s[^<>]*|\s*/)?\s*>")
INACTIVE = {"obsolete", "vanished"}
VOID_TAGS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input", "link",
    "meta", "param", "source", "track", "wbr",
}


class RichTextSignature(HTMLParser):
    """Keep tag order/nesting and link targets, allowing Qt's loose HTML syntax."""

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.tokens = []

    @staticmethod
    def targets(attrs):
        return tuple(sorted((name, value) for name, value in attrs
                            if name in {"href", "src"}))

    def handle_starttag(self, tag, attrs):
        self.tokens.append(("void" if tag in VOID_TAGS else "start",
                            tag, self.targets(attrs)))

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        if tag not in VOID_TAGS:
            self.handle_endtag(tag)

    def handle_endtag(self, tag):
        if tag not in VOID_TAGS:
            self.tokens.append(("end", tag, ()))


def html_signature(text):
    parser = RichTextSignature()
    parser.feed(text)
    parser.close()
    return parser.tokens


def element_text(element):
    return "".join(element.itertext()) if element is not None else ""


def short(text, limit=110):
    value = repr(text)
    return value if len(value) <= limit else value[:limit - 3] + "..."


def message_label(context, message, catalog):
    location = message.find("location")
    if location is not None:
        filename = location.get("filename", "")
        line = location.get("line", "")
        where = filename + (":" + line if line else "")
    else:
        where = str(catalog)
    return "{} [{}] {}".format(where, context, short(element_text(message.find("source"))))


def read_catalog(path, errors, validate=True):
    root = ET.parse(path).getroot()
    if root.tag != "TS":
        raise ValueError("{} is not a Qt TS catalog".format(path))
    entries = {}
    for context_node in root.findall("context"):
        context = element_text(context_node.find("name"))
        for message in context_node.findall("message"):
            translation = message.find("translation")
            if translation is not None and translation.get("type") in INACTIVE:
                continue
            source = element_text(message.find("source"))
            if not source:
                continue
            comment = element_text(message.find("comment"))
            key = (context, source, comment)
            label = message_label(context, message, path)
            if key in entries:
                errors.append("{}: duplicate active translation".format(label))
            forms = [] if translation is None else translation.findall("numerusform")
            values = [element_text(form) for form in forms] if forms else [element_text(translation)]
            finished = (translation is not None
                        and translation.get("type") not in INACTIVE | {"unfinished"}
                        and all(value.strip() for value in values))
            entries[key] = finished
            if not validate:
                continue
            if not finished:
                errors.append("{}: missing, empty or unfinished translation".format(label))
                continue
            expected_placeholders = Counter(PLACEHOLDER.findall(source))
            for index, value in enumerate(values, 1):
                form_label = label + (" (plural form {})".format(index) if forms else "")
                actual_placeholders = Counter(PLACEHOLDER.findall(value))
                if actual_placeholders != expected_placeholders:
                    errors.append("{}: placeholders differ: expected {}, got {}".format(
                        form_label, dict(expected_placeholders), dict(actual_placeholders)))
                if RICH_TEXT.search(source) or RICH_TEXT.search(value):
                    if html_signature(source) != html_signature(value):
                        errors.append("{}: HTML tag structure or link/image targets changed".format(form_label))
    return entries


def check_ui_coverage(source_root, entries, errors):
    paths = sorted(source_root.rglob("*.ui"))
    if not paths:
        errors.append("{}: no .ui files found".format(source_root))
    count = 0
    for path in paths:
        root = ET.parse(path).getroot()
        context = element_text(root.find("class"))
        parents = {child: parent for parent in root.iter() for child in parent}
        for node in root.iter("string"):
            source = element_text(node)
            if not source or node.get("notr", "").lower() == "true":
                continue
            count += 1
            key = (context, source, node.get("comment", ""))
            if entries.get(key):
                continue
            owner = parents.get(node)
            prop = owner.get("name", "") if owner is not None else ""
            while owner is not None and owner.tag not in {"widget", "action"}:
                owner = parents.get(owner)
            widget = owner.get("name", "") if owner is not None else ""
            errors.append("{} [{}] {}.{}: no finished translation for {}".format(
                path, context, widget, prop, short(source)))
    return len(paths), count


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--catalog", type=Path, default=ROOT / "translations/gqrx_zh_CN.ts",
                        help="Chinese TS catalog (default: translations/gqrx_zh_CN.ts)")
    parser.add_argument("--source-root", type=Path, default=ROOT / "src",
                        help="tree containing Qt Designer .ui sources (default: src)")
    parser.add_argument("--reference", type=Path,
                        help="optional fresh lupdate TS file for C++ and UI source coverage")
    args = parser.parse_args(argv)
    errors = []
    try:
        entries = read_catalog(args.catalog, errors)
        ui_files, ui_strings = check_ui_coverage(args.source_root, entries, errors)
        if args.reference:
            reference = read_catalog(args.reference, errors, validate=False)
            for key in sorted(reference):
                if not entries.get(key):
                    errors.append("{} [{}]: no finished translation for {}{}".format(
                        args.reference, key[0], short(key[1]),
                        " (comment: {})".format(key[2]) if key[2] else ""))
    except (OSError, ET.ParseError, ValueError) as exc:
        print("Translation check failed: {}".format(exc), file=sys.stderr)
        return 1
    if errors:
        for error in errors:
            print("ERROR: " + error, file=sys.stderr)
        print("Translation check failed: {} problem(s).".format(len(errors)), file=sys.stderr)
        return 1
    print("Translation check passed: {} active messages; {} .ui files, {} string occurrences{}.".format(
        len(entries), ui_files, ui_strings,
        "; fresh lupdate coverage checked" if args.reference else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
