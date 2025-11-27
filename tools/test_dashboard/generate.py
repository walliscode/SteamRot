#!/usr/bin/env python3
"""
generate.py - Main entry point for test dashboard generation

This is a placeholder for Phase 2-3 implementation. Currently provides
basic functionality to verify scanner and parser work correctly.

Usage:
    python -m tools.test_dashboard
    python tools/test_dashboard/generate.py --tests-dir tests/
"""
import argparse
import logging
import json
from pathlib import Path
from typing import List, Set

from .scanner import TestFileScanner
from .parser import TestDataParser, ParsedTest, parse_all_tests

logging.basicConfig(
    level=logging.INFO,
    format='%(levelname)s: %(message)s'
)
logger = logging.getLogger(__name__)


def collect_all_tags(tests: List[ParsedTest]) -> Set[str]:
    """Collect all unique tags across all tests."""
    tags = set()
    for test in tests:
        tags.update(test.metadata.tags)
    return tags


def collect_all_functions(tests: List[ParsedTest]) -> Set[str]:
    """Collect all unique function types across all tests."""
    functions = set()
    for test in tests:
        functions.update(test.functions_used)
    return functions


def collect_all_logic_classes(tests: List[ParsedTest]) -> Set[str]:
    """Collect all unique logic class types across all tests."""
    classes = set()
    for test in tests:
        classes.update(test.logic_classes_used)
    return classes


def print_summary(tests: List[ParsedTest]) -> None:
    """Print a summary of parsed test data."""
    print(f"\n{'='*60}")
    print(f"Test Data Dashboard - Phase 1 Scanner/Parser")
    print(f"{'='*60}\n")
    
    print(f"Total test files found: {len(tests)}")
    
    # Collect unique values
    all_tags = collect_all_tags(tests)
    all_functions = collect_all_functions(tests)
    all_logic_classes = collect_all_logic_classes(tests)
    
    print(f"Unique tags: {len(all_tags)}")
    print(f"Unique functions: {len(all_functions)}")
    print(f"Unique logic classes: {len(all_logic_classes)}")
    
    print(f"\n{'='*60}")
    print("Tags:")
    print(f"{'='*60}")
    for tag in sorted(all_tags):
        count = sum(1 for t in tests if tag in t.metadata.tags)
        print(f"  {tag}: {count} tests")
    
    print(f"\n{'='*60}")
    print("Functions:")
    print(f"{'='*60}")
    for func in sorted(all_functions):
        count = sum(1 for t in tests if func in t.functions_used)
        print(f"  {func}: {count} tests")
    
    print(f"\n{'='*60}")
    print("Logic Classes:")
    print(f"{'='*60}")
    for cls in sorted(all_logic_classes):
        count = sum(1 for t in tests if cls in t.logic_classes_used)
        print(f"  {cls}: {count} tests")
    
    print(f"\n{'='*60}")
    print("Tests by Category:")
    print(f"{'='*60}")
    categories = {}
    for test in tests:
        cat = test.category
        if cat not in categories:
            categories[cat] = []
        categories[cat].append(test)
    
    for cat in sorted(categories.keys()):
        print(f"\n  {cat}/ ({len(categories[cat])} tests)")
        for test in categories[cat][:5]:  # Show first 5
            print(f"    - {test.metadata.test_name}")
        if len(categories[cat]) > 5:
            print(f"    ... and {len(categories[cat]) - 5} more")
    
    print(f"\n{'='*60}")
    print("Phase 1 Complete - Scanner and Parser working!")
    print(f"{'='*60}\n")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Generate test data dashboard (Phase 1: Scanner/Parser)"
    )
    parser.add_argument(
        "--tests-dir",
        type=Path,
        default=Path("tests"),
        help="Root directory for test files (default: tests/)"
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("documentation/generated/test_dashboard.html"),
        help="Output HTML file path (not yet implemented)"
    )
    parser.add_argument(
        "--json",
        type=Path,
        help="Export parsed data as JSON to this file"
    )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose logging"
    )
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Initialize scanner and parser
    try:
        scanner = TestFileScanner(args.tests_dir)
    except ValueError as e:
        logger.error(str(e))
        return 1
    
    parser_instance = TestDataParser()
    
    # Parse all test files
    tests = parse_all_tests(scanner, parser_instance)
    
    if not tests:
        logger.warning("No test files found!")
        return 1
    
    # Print summary
    print_summary(tests)
    
    # Export JSON if requested
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        data = {
            "tests": [t.to_dict() for t in tests],
            "summary": {
                "total_tests": len(tests),
                "tags": sorted(collect_all_tags(tests)),
                "functions": sorted(collect_all_functions(tests)),
                "logic_classes": sorted(collect_all_logic_classes(tests))
            }
        }
        with open(args.json, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2)
        logger.info(f"Exported parsed data to {args.json}")
    
    return 0


if __name__ == "__main__":
    exit(main())
