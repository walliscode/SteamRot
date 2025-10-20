#!/usr/bin/env python3
"""
Validate test data JSON files against the test data schema.

Usage:
    python3 validate_test_data.py [file_or_directory]
    
Examples:
    # Validate a single file
    python3 validate_test_data.py component_configs/cmeta_test_data.json
    
    # Validate all files in a directory
    python3 validate_test_data.py component_configs/
    
    # Validate all test data files (default)
    python3 validate_test_data.py
"""

import json
import sys
from pathlib import Path
from typing import List, Tuple


def load_json_file(file_path: Path) -> dict:
    """Load and parse a JSON file."""
    try:
        with open(file_path, 'r') as f:
            return json.load(f)
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON in {file_path}: {e}")
    except Exception as e:
        raise ValueError(f"Error reading {file_path}: {e}")


def validate_test_case(test_case: dict, index: int) -> List[str]:
    """Validate a single test case structure."""
    errors = []
    
    # Required fields
    if 'name' not in test_case:
        errors.append(f"Test case {index}: Missing required field 'name'")
    if 'input' not in test_case:
        errors.append(f"Test case {index}: Missing required field 'input'")
    if 'expected' not in test_case:
        errors.append(f"Test case {index}: Missing required field 'expected'")
    
    # Optional fields type checking
    if 'description' in test_case and not isinstance(test_case['description'], str):
        errors.append(f"Test case {index}: 'description' must be a string")
    if 'tags' in test_case and not isinstance(test_case['tags'], list):
        errors.append(f"Test case {index}: 'tags' must be an array")
    if 'should_fail' in test_case and not isinstance(test_case['should_fail'], bool):
        errors.append(f"Test case {index}: 'should_fail' must be a boolean")
    
    return errors


def validate_test_data(data: dict, file_path: Path) -> List[str]:
    """Validate test data against the schema requirements."""
    errors = []
    
    # Required top-level fields
    if 'test_suite' not in data:
        errors.append("Missing required field 'test_suite'")
    elif not isinstance(data['test_suite'], str):
        errors.append("'test_suite' must be a string")
    
    if 'test_cases' not in data:
        errors.append("Missing required field 'test_cases'")
    elif not isinstance(data['test_cases'], list):
        errors.append("'test_cases' must be an array")
    elif len(data['test_cases']) == 0:
        errors.append("'test_cases' array must have at least one item")
    else:
        # Validate each test case
        for idx, test_case in enumerate(data['test_cases']):
            test_case_errors = validate_test_case(test_case, idx)
            errors.extend(test_case_errors)
    
    # Optional field type checking
    if 'description' in data and not isinstance(data['description'], str):
        errors.append("'description' must be a string")
    
    return errors


def validate_file(file_path: Path) -> Tuple[bool, List[str]]:
    """Validate a single test data file."""
    try:
        data = load_json_file(file_path)
        errors = validate_test_data(data, file_path)
        
        if errors:
            return False, errors
        return True, []
    except Exception as e:
        return False, [str(e)]


def find_test_data_files(path: Path) -> List[Path]:
    """Find all test data JSON files in the given path."""
    if path.is_file():
        if path.suffix == '.json' and path.name != 'test_data_schema.json':
            return [path]
        return []
    
    # Find all JSON files recursively, excluding schema file
    json_files = []
    for json_file in path.rglob('*.json'):
        if json_file.name != 'test_data_schema.json':
            json_files.append(json_file)
    return json_files


def main():
    """Main validation function."""
    # Determine path to validate
    if len(sys.argv) > 1:
        target_path = Path(sys.argv[1])
    else:
        # Default to current directory
        target_path = Path(__file__).parent
    
    if not target_path.exists():
        print(f"Error: Path '{target_path}' does not exist")
        sys.exit(1)
    
    # Find files to validate
    files_to_validate = find_test_data_files(target_path)
    
    if not files_to_validate:
        print(f"No test data files found in '{target_path}'")
        sys.exit(0)
    
    # Validate each file
    all_valid = True
    results = []
    
    for file_path in sorted(files_to_validate):
        is_valid, errors = validate_file(file_path)
        relative_path = file_path.relative_to(Path(__file__).parent)
        
        if is_valid:
            results.append(f"✓ {relative_path}")
        else:
            results.append(f"✗ {relative_path}")
            for error in errors:
                results.append(f"    {error}")
            all_valid = False
    
    # Print results
    print(f"Validating {len(files_to_validate)} test data file(s)...")
    print()
    for result in results:
        print(result)
    
    print()
    if all_valid:
        print(f"✓ All {len(files_to_validate)} file(s) are valid!")
        sys.exit(0)
    else:
        valid_count = sum(1 for r in results if r.startswith('✓'))
        invalid_count = len(files_to_validate) - valid_count
        print(f"✗ {invalid_count} file(s) have validation errors")
        sys.exit(1)


if __name__ == '__main__':
    main()
