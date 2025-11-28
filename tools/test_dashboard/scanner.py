#!/usr/bin/env python3
"""
scanner.py - Recursively find and load test_data.json files

This module provides functionality to scan directories for test data files
and prepare them for parsing.
"""
from pathlib import Path
from typing import List
import logging

logger = logging.getLogger(__name__)


class TestFileScanner:
    """Scanner for finding test_data.json files in a directory tree."""
    
    def __init__(self, root_dir: Path):
        """
        Initialize the scanner with a root directory.
        
        Args:
            root_dir: The root directory to scan for test data files.
        """
        self.root_dir = Path(root_dir)
        if not self.root_dir.exists():
            raise ValueError(f"Root directory does not exist: {root_dir}")
        
    def find_test_files(self) -> List[Path]:
        """
        Find all .test_data.json files recursively.
        
        Returns:
            A sorted list of Path objects for each test data file found.
        """
        pattern = "*.test_data.json"
        files = list(self.root_dir.rglob(pattern))
        logger.info(f"Found {len(files)} test data files in {self.root_dir}")
        return sorted(files)
    
    def get_relative_path(self, file_path: Path) -> str:
        """
        Get path relative to root for display.
        
        Args:
            file_path: The absolute path to convert.
            
        Returns:
            The path relative to the root directory as a string.
        """
        try:
            return str(file_path.relative_to(self.root_dir))
        except ValueError:
            # File is not under root_dir, return absolute path
            return str(file_path)
    
    def get_category(self, file_path: Path) -> str:
        """
        Extract the category from the file path (e.g., 'harness', 'unit/logic').
        
        Args:
            file_path: The path to the test data file.
            
        Returns:
            The category derived from the directory structure.
        """
        relative = self.get_relative_path(file_path)
        parts = Path(relative).parts
        
        # Remove 'data' directory and filename
        category_parts = [p for p in parts[:-1] if p != 'data']
        
        if category_parts:
            return '/'.join(category_parts)
        return 'uncategorized'
