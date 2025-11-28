#!/usr/bin/env python3
"""
html_generator.py - Generate HTML dashboard from parsed test data

This module handles the HTML generation, including template loading,
CSS/JS embedding, and building the interactive dashboard.
"""
import html
import json
from datetime import datetime
from pathlib import Path
from typing import List, Dict, Set, Any
from collections import defaultdict

from .parser import ParsedTest


class HtmlGenerator:
    """Generates HTML dashboard from parsed test data."""
    
    def __init__(self):
        """Initialize the generator with template paths."""
        self.template_dir = Path(__file__).parent / "templates"
        self.static_dir = Path(__file__).parent / "static"
    
    def generate(self, tests: List[ParsedTest], output_path: Path) -> None:
        """
        Generate the HTML dashboard and write to output file.
        
        Args:
            tests: List of parsed test data
            output_path: Path to write the HTML file
        """
        # Collect all unique values
        all_tags = self._collect_unique(tests, lambda t: t.metadata.tags)
        all_functions = self._collect_unique(tests, lambda t: t.functions_used)
        all_logic_classes = self._collect_unique(tests, lambda t: t.logic_classes_used)
        all_categories = self._collect_unique(tests, lambda t: [t.category])
        
        # Build coverage matrix
        coverage_matrix = self._build_coverage_matrix(tests)
        
        # Load template and assets
        template = self._load_template("base.html")
        css_content = self._load_static("styles.css")
        js_content = self._load_static("dashboard.js")
        
        # Generate component HTML
        tag_checkboxes = self._generate_checkboxes(sorted(all_tags), "tag")
        function_checkboxes = self._generate_checkboxes(sorted(all_functions), "function")
        logic_class_checkboxes = self._generate_checkboxes(sorted(all_logic_classes), "logic-class")
        category_checkboxes = self._generate_checkboxes(sorted(all_categories), "category")
        coverage_matrix_html = self._generate_coverage_matrix_html(
            coverage_matrix, sorted(all_functions), sorted(all_logic_classes)
        )
        test_rows_html = self._generate_test_rows(tests)
        graph_start_options = self._generate_graph_start_options(sorted(all_functions))
        
        # Convert tests to JSON for JavaScript
        tests_json = json.dumps([t.to_dict() for t in tests], indent=2)
        
        # Fill template
        html_content = template.replace("{{CSS_CONTENT}}", css_content)
        html_content = html_content.replace("{{JAVASCRIPT_CODE}}", js_content)
        html_content = html_content.replace("{{TEST_DATA_JSON}}", tests_json)
        html_content = html_content.replace("{{TOTAL_TESTS}}", str(len(tests)))
        html_content = html_content.replace("{{TOTAL_FUNCTIONS}}", str(len(all_functions)))
        html_content = html_content.replace("{{TOTAL_LOGIC_CLASSES}}", str(len(all_logic_classes)))
        html_content = html_content.replace("{{TOTAL_TAGS}}", str(len(all_tags)))
        html_content = html_content.replace("{{TIMESTAMP}}", datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
        html_content = html_content.replace("{{TAG_CHECKBOXES}}", tag_checkboxes)
        html_content = html_content.replace("{{FUNCTION_CHECKBOXES}}", function_checkboxes)
        html_content = html_content.replace("{{LOGIC_CLASS_CHECKBOXES}}", logic_class_checkboxes)
        html_content = html_content.replace("{{CATEGORY_CHECKBOXES}}", category_checkboxes)
        html_content = html_content.replace("{{COVERAGE_MATRIX_HTML}}", coverage_matrix_html)
        html_content = html_content.replace("{{TEST_ROWS}}", test_rows_html)
        html_content = html_content.replace("{{GRAPH_START_OPTIONS}}", graph_start_options)
        
        # Write output
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(html_content)
    
    def _load_template(self, name: str) -> str:
        """Load an HTML template file."""
        path = self.template_dir / name
        if not path.exists():
            raise FileNotFoundError(f"Template not found: {path}")
        return path.read_text(encoding='utf-8')
    
    def _load_static(self, name: str) -> str:
        """Load a static asset file (CSS/JS)."""
        path = self.static_dir / name
        if not path.exists():
            raise FileNotFoundError(f"Static file not found: {path}")
        return path.read_text(encoding='utf-8')
    
    def _collect_unique(self, tests: List[ParsedTest], extractor) -> Set[str]:
        """Collect unique values from tests using an extractor function."""
        values = set()
        for test in tests:
            values.update(extractor(test))
        return values
    
    def _build_coverage_matrix(self, tests: List[ParsedTest]) -> Dict[str, Dict[str, List[str]]]:
        """
        Build a coverage matrix mapping function -> logic_class -> test names.
        
        For tests without functions or logic classes, uses special "(none)" key.
        """
        matrix = defaultdict(lambda: defaultdict(list))
        
        for test in tests:
            functions = test.functions_used if test.functions_used else ["(none)"]
            logic_classes = test.logic_classes_used if test.logic_classes_used else ["(none)"]
            
            for func in functions:
                for cls in logic_classes:
                    matrix[func][cls].append(test.metadata.test_name)
        
        return matrix
    
    def _generate_checkboxes(self, values: List[str], prefix: str) -> str:
        """Generate filter checkbox HTML for a list of values."""
        import re
        
        if not values:
            return '<span class="empty-state">None available</span>'
        
        lines = []
        for value in values:
            escaped_value = html.escape(value)
            # Use a safe ID by replacing all non-alphanumeric characters
            safe_id = f"{prefix}-{re.sub(r'[^a-zA-Z0-9_-]', '-', value)}"
            lines.append(
                f'<label><input type="checkbox" value="{escaped_value}" id="{html.escape(safe_id)}"><span>{escaped_value}</span></label>'
            )
        return '\n                    '.join(lines)
    
    def _generate_graph_start_options(self, functions: List[str]) -> str:
        """Generate the initial options for the graph explorer dropdown.
        
        Note: The JavaScript dynamically updates options based on the selected type.
        This just provides the initial function options when the page loads.
        """
        if not functions:
            return ''
        
        lines = []
        for func in functions:
            escaped_func = html.escape(func)
            lines.append(f'<option value="{escaped_func}">{escaped_func}</option>')
        return '\n                        '.join(lines)
    
    def _generate_coverage_matrix_html(
        self, 
        matrix: Dict[str, Dict[str, List[str]]], 
        functions: List[str],
        logic_classes: List[str]
    ) -> str:
        """Generate the HTML table for the coverage matrix."""
        # Ensure we have at least something to show
        if not functions and not logic_classes:
            return '<p class="empty-state">No functions or logic classes found in test data.</p>'
        
        # Add (none) to handle tests without functions/logic classes
        display_functions = functions if functions else ["(none)"]
        display_classes = logic_classes if logic_classes else ["(none)"]
        
        # If there are tests without functions or classes, add (none)
        has_none_function = "(none)" in matrix
        has_none_class = any("(none)" in matrix.get(f, {}) for f in matrix)
        
        if has_none_function and "(none)" not in display_functions:
            display_functions = ["(none)"] + display_functions
        if has_none_class and "(none)" not in display_classes:
            display_classes = ["(none)"] + display_classes
        
        lines = ['<table>']
        
        # Header row
        lines.append('<tr>')
        lines.append('<th class="row-header">Function \\ Logic Class</th>')
        for cls in display_classes:
            escaped_cls = html.escape(cls)
            lines.append(f'<th>{escaped_cls}</th>')
        lines.append('</tr>')
        
        # Data rows
        for func in display_functions:
            escaped_func = html.escape(func)
            lines.append('<tr>')
            lines.append(f'<th class="row-header">{escaped_func}</th>')
            
            for cls in display_classes:
                test_names = matrix.get(func, {}).get(cls, [])
                count = len(test_names)
                
                if count > 0:
                    # Has tests - make it clickable
                    data_func = html.escape(func if func != "(none)" else "none")
                    data_cls = html.escape(cls if cls != "(none)" else "none")
                    tooltip = html.escape(", ".join(test_names[:5]))
                    if count > 5:
                        tooltip += f" (+{count - 5} more)"
                    lines.append(
                        f'<td class="has-tests" data-function="{data_func}" '
                        f'data-logic-class="{data_cls}" title="{tooltip}">{count}</td>'
                    )
                else:
                    lines.append('<td class="empty-cell">-</td>')
            
            lines.append('</tr>')
        
        lines.append('</table>')
        return '\n                    '.join(lines)
    
    def _generate_test_rows(self, tests: List[ParsedTest]) -> str:
        """Generate HTML table rows for all tests."""
        lines = []
        
        for test in tests:
            escaped_name = html.escape(test.metadata.test_name)
            escaped_category = html.escape(test.category)
            
            # Tags
            tags_html = ' '.join(
                f'<span class="tag">{html.escape(t)}</span>' 
                for t in test.metadata.tags
            ) or '-'
            
            # Functions
            functions_html = ' '.join(
                f'<span class="tag function-tag">{html.escape(f)}</span>' 
                for f in test.functions_used
            ) or '-'
            
            # Logic classes
            classes_html = ' '.join(
                f'<span class="tag logic-class-tag">{html.escape(c)}</span>' 
                for c in test.logic_classes_used
            ) or '-'
            
            lines.append(f'''<tr data-test-id="{escaped_name}">
                        <td>{escaped_name}</td>
                        <td><span class="tag category-tag">{escaped_category}</span></td>
                        <td>{tags_html}</td>
                        <td>{functions_html}</td>
                        <td>{classes_html}</td>
                        <td>{test.num_ticks}</td>
                    </tr>''')
        
        return '\n                    '.join(lines)
