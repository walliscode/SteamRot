#!/usr/bin/env python3
"""
parser.py - Parse and normalize test_data.json files

This module provides data classes and parsing functionality for test data files,
extracting metadata, simulation steps, and other relevant information.
"""
import json
import html
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Dict, Any, Optional
import logging

logger = logging.getLogger(__name__)


@dataclass
class TestMetadata:
    """Metadata about a test case."""
    test_name: str
    description: str = ""
    tags: List[str] = field(default_factory=list)
    expected_to_pass: bool = True
    author: str = ""
    version: int = 1


@dataclass
class SimulationStep:
    """A single step in a simulation sequence."""
    simulation_type: str  # Action, Movement, Render, Collision
    execution_mode: str   # Function, LogicClass
    function_type: Optional[str] = None
    logic_class_type: Optional[str] = None
    description: str = ""


@dataclass
class ParsedTest:
    """Fully parsed test data with extracted information."""
    file_path: str
    relative_path: str
    category: str
    metadata: TestMetadata
    simulation_steps: List[SimulationStep] = field(default_factory=list)
    functions_used: List[str] = field(default_factory=list)
    logic_classes_used: List[str] = field(default_factory=list)
    num_ticks: int = 1
    has_input_sequence: bool = False
    has_event_sequence: bool = False
    has_tick_snapshots: bool = False
    has_start_data: bool = False
    has_expected_data: bool = False
    raw_data: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self, escape_html: bool = False) -> Dict[str, Any]:
        """
        Convert to a dictionary suitable for JSON serialization.
        
        Args:
            escape_html: If True, escape HTML characters for safe rendering.
                        Should be True when embedding in HTML, False for JSON export.
        """
        def escape(s: str) -> str:
            return html.escape(s) if escape_html else s
        
        return {
            "file_path": self.file_path,
            "relative_path": self.relative_path,
            "category": self.category,
            "name": escape(self.metadata.test_name),
            "description": escape(self.metadata.description),
            "tags": [escape(t) for t in self.metadata.tags],
            "expected_to_pass": self.metadata.expected_to_pass,
            "author": escape(self.metadata.author),
            "version": self.metadata.version,
            "functions": [escape(f) for f in self.functions_used],
            "logic_classes": [escape(c) for c in self.logic_classes_used],
            "num_ticks": self.num_ticks,
            "has_input_sequence": self.has_input_sequence,
            "has_event_sequence": self.has_event_sequence,
            "has_tick_snapshots": self.has_tick_snapshots,
            "has_start_data": self.has_start_data,
            "has_expected_data": self.has_expected_data,
            "simulation_steps": [
                {
                    "simulation_type": escape(s.simulation_type),
                    "execution_mode": escape(s.execution_mode),
                    "function_type": escape(s.function_type) if s.function_type else None,
                    "logic_class_type": escape(s.logic_class_type) if s.logic_class_type else None,
                    "description": escape(s.description)
                } for s in self.simulation_steps
            ],
            "raw": self.raw_data
        }


class TestDataParser:
    """Parser for test_data.json files."""
    
    def parse_file(self, file_path: Path, relative_path: str, category: str) -> ParsedTest:
        """
        Parse a single test_data.json file.
        
        Args:
            file_path: Absolute path to the file.
            relative_path: Path relative to tests directory.
            category: Category derived from directory structure.
            
        Returns:
            A ParsedTest object with all extracted data.
            
        Raises:
            json.JSONDecodeError: If the file contains invalid JSON.
            KeyError: If required fields are missing.
        """
        logger.debug(f"Parsing {file_path}")
        
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        metadata = self._parse_metadata(data.get("metadata", {}))
        steps = self._parse_simulation_steps(data.get("simulation_data", {}))
        
        return ParsedTest(
            file_path=str(file_path),
            relative_path=relative_path,
            category=category,
            metadata=metadata,
            simulation_steps=steps,
            functions_used=self._extract_functions(steps),
            logic_classes_used=self._extract_logic_classes(steps),
            num_ticks=data.get("num_ticks", 1) or 1,
            has_input_sequence="input_sequence" in data and data["input_sequence"] is not None,
            has_event_sequence="event_sequence" in data and data["event_sequence"] is not None,
            has_tick_snapshots="tick_snapshots" in data and data["tick_snapshots"] is not None,
            has_start_data="start_data_collection" in data and data["start_data_collection"] is not None,
            has_expected_data="expected_data_collection" in data and data["expected_data_collection"] is not None,
            raw_data=data
        )
    
    def _parse_metadata(self, data: Dict) -> TestMetadata:
        """Parse the metadata section of a test data file."""
        return TestMetadata(
            test_name=data.get("test_name", "Unknown"),
            description=data.get("description", ""),
            tags=data.get("tags", []) or [],
            expected_to_pass=data.get("expected_to_pass", True),
            author=data.get("author", ""),
            version=data.get("version", 1)
        )
    
    def _parse_simulation_steps(self, data: Dict) -> List[SimulationStep]:
        """Parse the simulation_data.steps section."""
        steps = []
        for step_data in data.get("steps", []) or []:
            steps.append(SimulationStep(
                simulation_type=step_data.get("simulation_type", ""),
                execution_mode=step_data.get("execution_mode", ""),
                function_type=step_data.get("function_type"),
                logic_class_type=step_data.get("logic_class_type"),
                description=step_data.get("description", "")
            ))
        return steps
    
    def _extract_functions(self, steps: List[SimulationStep]) -> List[str]:
        """Extract unique function types from simulation steps."""
        functions = set()
        for step in steps:
            if step.execution_mode == "Function" and step.function_type:
                functions.add(step.function_type)
        return sorted(functions)
    
    def _extract_logic_classes(self, steps: List[SimulationStep]) -> List[str]:
        """Extract unique logic class types from simulation steps."""
        classes = set()
        for step in steps:
            if step.execution_mode == "LogicClass" and step.logic_class_type:
                classes.add(step.logic_class_type)
        return sorted(classes)


def parse_all_tests(scanner, parser) -> List[ParsedTest]:
    """
    Parse all test files found by the scanner.
    
    Args:
        scanner: A TestFileScanner instance.
        parser: A TestDataParser instance.
        
    Returns:
        A list of ParsedTest objects.
    """
    tests = []
    files = scanner.find_test_files()
    
    for file_path in files:
        try:
            relative = scanner.get_relative_path(file_path)
            category = scanner.get_category(file_path)
            test = parser.parse_file(file_path, relative, category)
            tests.append(test)
        except (json.JSONDecodeError, KeyError) as e:
            logger.warning(f"Failed to parse {file_path}: {e}")
    
    logger.info(f"Successfully parsed {len(tests)} of {len(files)} test files")
    return tests
