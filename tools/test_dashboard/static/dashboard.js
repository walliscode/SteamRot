/**
 * SteamRot Test Data Dashboard - Interactive JavaScript
 * Provides search, filtering, sorting, and detail view functionality
 */

// State management
let currentSort = { column: 'name', ascending: true };
let activeFilters = {
    search: '',
    tags: [],
    functions: [],
    logicClasses: [],
    categories: []
};

// Graph explorer state - Simulation Path Explorer
// Allows step-by-step exploration of simulation sequences from test data.
// pathTree is a prefix tree (trie) where:
// - Each node represents a step in a simulation sequence
// - children: Map<stepName, childNode> maps step names to child nodes
// - tests: Array of test names that end at this exact step
// - isEnd: Boolean indicating if any test ends at this node
// - isFunction: Boolean true if step is a Function, false if LogicClass
let graphState = {
    selectedPath: [],      // Array of selected step names in order
    pathTree: null,        // Prefix tree structure of all simulation paths
    matchingTests: []      // Tests that match the current path
};

// Initialize dashboard
document.addEventListener('DOMContentLoaded', () => {
    initializeEventListeners();
    initializeGraphExplorer();
    updateVisibleCount();
});

function initializeEventListeners() {
    // Search functionality
    const searchInput = document.getElementById('search');
    if (searchInput) {
        searchInput.addEventListener('input', debounce((e) => {
            activeFilters.search = e.target.value.toLowerCase();
            filterTests();
        }, 200));
    }
    
    // Clear filters button
    const clearBtn = document.getElementById('clear-filters');
    if (clearBtn) {
        clearBtn.addEventListener('click', clearAllFilters);
    }
    
    // Filter checkboxes
    document.querySelectorAll('#tag-filters input').forEach(cb => {
        cb.addEventListener('change', () => {
            activeFilters.tags = getCheckedValues('tag-filters');
            filterTests();
        });
    });
    
    document.querySelectorAll('#function-filters input').forEach(cb => {
        cb.addEventListener('change', () => {
            activeFilters.functions = getCheckedValues('function-filters');
            filterTests();
        });
    });
    
    document.querySelectorAll('#logic-class-filters input').forEach(cb => {
        cb.addEventListener('change', () => {
            activeFilters.logicClasses = getCheckedValues('logic-class-filters');
            filterTests();
        });
    });
    
    document.querySelectorAll('#category-filters input').forEach(cb => {
        cb.addEventListener('change', () => {
            activeFilters.categories = getCheckedValues('category-filters');
            filterTests();
        });
    });
    
    // Table sorting
    document.querySelectorAll('#tests-table th.sortable').forEach(th => {
        th.addEventListener('click', () => sortTable(th.dataset.sort));
    });
    
    // Row click for details
    document.querySelectorAll('#tests-table tbody tr').forEach(row => {
        row.addEventListener('click', () => showDetails(row.dataset.testId));
    });
    
    // Coverage matrix cell click
    document.querySelectorAll('#coverage-matrix td.has-tests').forEach(cell => {
        cell.addEventListener('click', () => {
            filterByCell(cell.dataset.function, cell.dataset.logicClass);
        });
    });
    
    // Close details panel
    const closeBtn = document.getElementById('close-details');
    if (closeBtn) {
        closeBtn.addEventListener('click', hideDetails);
    }
    
    // Overlay click closes details
    const overlay = document.getElementById('overlay');
    if (overlay) {
        overlay.addEventListener('click', hideDetails);
    }
    
    // Escape key closes details
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape') {
            hideDetails();
        }
    });
}

// Debounce utility for search
function debounce(func, wait) {
    let timeout;
    return function executedFunction(...args) {
        const later = () => {
            clearTimeout(timeout);
            func(...args);
        };
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
    };
}

// Get checked filter values
function getCheckedValues(containerId) {
    return Array.from(document.querySelectorAll(`#${containerId} input:checked`))
        .map(cb => cb.value);
}

// Filter tests based on all active filters
function filterTests() {
    let visibleCount = 0;
    
    document.querySelectorAll('#tests-table tbody tr').forEach(row => {
        const testId = row.dataset.testId;
        const test = TEST_DATA.find(t => t.name === testId);
        
        if (!test) {
            row.classList.add('hidden');
            return;
        }
        
        const visible = matchesFilters(test);
        row.classList.toggle('hidden', !visible);
        
        if (visible) visibleCount++;
    });
    
    updateVisibleCount(visibleCount);
}

// Check if a test matches all active filters
function matchesFilters(test) {
    // Search match
    if (activeFilters.search) {
        const searchable = `${test.name} ${test.description} ${test.tags.join(' ')} ${test.category}`.toLowerCase();
        if (!searchable.includes(activeFilters.search)) return false;
    }
    
    // Tag filter (OR logic - matches if ANY selected tag is present)
    if (activeFilters.tags.length > 0) {
        if (!activeFilters.tags.some(t => test.tags.includes(t))) return false;
    }
    
    // Function filter (OR logic)
    if (activeFilters.functions.length > 0) {
        if (!activeFilters.functions.some(f => test.functions.includes(f))) return false;
    }
    
    // Logic class filter (OR logic)
    if (activeFilters.logicClasses.length > 0) {
        if (!activeFilters.logicClasses.some(c => test.logic_classes.includes(c))) return false;
    }
    
    // Category filter (OR logic)
    if (activeFilters.categories.length > 0) {
        if (!activeFilters.categories.includes(test.category)) return false;
    }
    
    return true;
}

// Update visible count display
function updateVisibleCount(count) {
    if (count === undefined) {
        count = document.querySelectorAll('#tests-table tbody tr:not(.hidden)').length;
    }
    
    const countEl = document.getElementById('visible-count');
    if (countEl) countEl.textContent = count;
    
    const resultsCountEl = document.getElementById('results-count');
    if (resultsCountEl) resultsCountEl.textContent = count;
}

// Clear all filters
function clearAllFilters() {
    // Clear search
    const searchInput = document.getElementById('search');
    if (searchInput) searchInput.value = '';
    
    // Uncheck all checkboxes
    document.querySelectorAll('.filter-options input').forEach(cb => {
        cb.checked = false;
    });
    
    // Reset filter state
    activeFilters = {
        search: '',
        tags: [],
        functions: [],
        logicClasses: [],
        categories: []
    };
    
    filterTests();
}

// Sort table by column
function sortTable(column) {
    const tbody = document.querySelector('#tests-table tbody');
    const rows = Array.from(tbody.querySelectorAll('tr'));
    
    // Toggle sort direction if same column
    if (currentSort.column === column) {
        currentSort.ascending = !currentSort.ascending;
    } else {
        currentSort.column = column;
        currentSort.ascending = true;
    }
    
    // Update sort icons
    document.querySelectorAll('#tests-table th.sortable').forEach(th => {
        const icon = th.querySelector('.sort-icon');
        if (icon) {
            if (th.dataset.sort === column) {
                icon.textContent = currentSort.ascending ? '↑' : '↓';
            } else {
                icon.textContent = '↕';
            }
        }
    });
    
    // Sort rows
    rows.sort((a, b) => {
        const testA = TEST_DATA.find(t => t.name === a.dataset.testId);
        const testB = TEST_DATA.find(t => t.name === b.dataset.testId);
        
        if (!testA || !testB) return 0;
        
        let valA, valB;
        
        switch (column) {
            case 'name':
                valA = testA.name.toLowerCase();
                valB = testB.name.toLowerCase();
                break;
            case 'category':
                valA = testA.category.toLowerCase();
                valB = testB.category.toLowerCase();
                break;
            case 'ticks':
                valA = testA.num_ticks;
                valB = testB.num_ticks;
                break;
            default:
                return 0;
        }
        
        if (valA < valB) return currentSort.ascending ? -1 : 1;
        if (valA > valB) return currentSort.ascending ? 1 : -1;
        return 0;
    });
    
    // Reorder DOM
    rows.forEach(row => tbody.appendChild(row));
}

// Filter by coverage matrix cell
function filterByCell(func, logicClass) {
    // Clear all filters first
    clearAllFilters();
    
    // Set specific filters based on cell data
    if (func && func !== 'none') {
        const funcCheckbox = document.querySelector(`#function-filters input[value="${func}"]`);
        if (funcCheckbox) {
            funcCheckbox.checked = true;
            activeFilters.functions = [func];
        }
    }
    
    if (logicClass && logicClass !== 'none') {
        const classCheckbox = document.querySelector(`#logic-class-filters input[value="${logicClass}"]`);
        if (classCheckbox) {
            classCheckbox.checked = true;
            activeFilters.logicClasses = [logicClass];
        }
    }
    
    filterTests();
    
    // Scroll to test list
    document.getElementById('test-list').scrollIntoView({ behavior: 'smooth' });
}

// Show test details panel
function showDetails(testName) {
    const test = TEST_DATA.find(t => t.name === testName);
    if (!test) return;
    
    const content = document.getElementById('details-content');
    const title = document.getElementById('details-title');
    
    if (title) title.textContent = test.name;
    
    // Build details HTML
    let html = `
        <div class="detail-row">
            <span class="detail-label">File Path</span>
            <span class="detail-value">${test.relative_path}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Category</span>
            <span class="detail-value"><span class="tag category-tag">${test.category}</span></span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Description</span>
            <span class="detail-value">${test.description || 'No description provided'}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Expected to Pass</span>
            <span class="detail-value">${test.expected_to_pass ? '✅ Yes' : '❌ No'}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Number of Ticks</span>
            <span class="detail-value">${test.num_ticks}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Tags</span>
            <span class="detail-value">${test.tags.length > 0 ? test.tags.map(t => `<span class="tag">${t}</span>`).join(' ') : 'None'}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Functions Used</span>
            <span class="detail-value">${test.functions.length > 0 ? test.functions.map(f => `<span class="tag function-tag">${f}</span>`).join(' ') : 'None'}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Logic Classes Used</span>
            <span class="detail-value">${test.logic_classes.length > 0 ? test.logic_classes.map(c => `<span class="tag logic-class-tag">${c}</span>`).join(' ') : 'None'}</span>
        </div>
        
        <div class="detail-row">
            <span class="detail-label">Data Features</span>
            <span class="detail-value">
                ${test.has_input_sequence ? '<span class="tag">Has Inputs</span>' : ''}
                ${test.has_event_sequence ? '<span class="tag">Has Events</span>' : ''}
                ${test.has_tick_snapshots ? '<span class="tag">Has Snapshots</span>' : ''}
                ${test.has_start_data ? '<span class="tag">Has Start Data</span>' : ''}
                ${test.has_expected_data ? '<span class="tag">Has Expected Data</span>' : ''}
                ${!test.has_input_sequence && !test.has_event_sequence && !test.has_tick_snapshots && !test.has_start_data && !test.has_expected_data ? 'None' : ''}
            </span>
        </div>
    `;
    
    // Simulation steps
    if (test.simulation_steps && test.simulation_steps.length > 0) {
        html += `<h3>Simulation Steps (${test.simulation_steps.length})</h3>`;
        test.simulation_steps.forEach((step, i) => {
            const target = step.function_type || step.logic_class_type || 'N/A';
            html += `
                <div class="simulation-step">
                    <div class="step-header">
                        <span class="step-type">${step.simulation_type}</span>
                        Step ${i + 1}: ${step.execution_mode}
                    </div>
                    <div class="step-mode">Target: <strong>${target}</strong></div>
                    ${step.description ? `<div class="step-description">${step.description}</div>` : ''}
                </div>
            `;
        });
    }
    
    // Raw JSON (collapsible)
    html += `
        <h3>Raw JSON Data</h3>
        <pre class="raw-json">${JSON.stringify(test.raw, null, 2)}</pre>
    `;
    
    if (content) content.innerHTML = html;
    
    // Show panel and overlay
    document.getElementById('test-details').classList.remove('hidden');
    document.getElementById('overlay').classList.remove('hidden');
    document.body.style.overflow = 'hidden';
}

// Hide test details panel
function hideDetails() {
    document.getElementById('test-details').classList.add('hidden');
    document.getElementById('overlay').classList.add('hidden');
    document.body.style.overflow = '';
}

// ============================================
// Graph Explorer Functions - Simulation Path Explorer
// ============================================

function initializeGraphExplorer() {
    const resetBtn = document.getElementById('graph-reset');
    
    if (resetBtn) {
        resetBtn.addEventListener('click', resetGraph);
    }
    
    // Build the simulation path tree from test data
    buildPathTree();
    
    // Render the initial state (first steps only)
    renderGraph();
}

/**
 * Build a trie/tree structure from all simulation step sequences.
 * Each node has: { children: Map<stepName, node>, tests: [testName], isFunction: bool }
 */
function buildPathTree() {
    graphState.pathTree = {
        children: new Map(),
        tests: [],
        isEnd: false
    };
    
    TEST_DATA.forEach(test => {
        // Get simulation steps for this test
        const steps = test.simulation_steps || [];
        if (steps.length === 0) return;
        
        let currentNode = graphState.pathTree;
        
        steps.forEach((step, index) => {
            // Use execution_mode to determine which field to use for step name
            const isFunction = step.execution_mode === 'Function';
            const stepName = isFunction ? step.function_type : step.logic_class_type;
            if (!stepName) return;
            
            if (!currentNode.children.has(stepName)) {
                currentNode.children.set(stepName, {
                    children: new Map(),
                    tests: [],
                    isEnd: false,
                    isFunction: isFunction,
                    description: step.description || ''
                });
            }
            
            currentNode = currentNode.children.get(stepName);
            
            // If this is the last step, mark as end and add test
            if (index === steps.length - 1) {
                currentNode.isEnd = true;
                currentNode.tests.push(test.name);
            }
        });
    });
}

/**
 * Get the current node in the path tree based on selected path
 */
function getCurrentPathNode() {
    let currentNode = graphState.pathTree;
    
    for (const stepName of graphState.selectedPath) {
        if (currentNode.children.has(stepName)) {
            currentNode = currentNode.children.get(stepName);
        } else {
            return null; // Path not found
        }
    }
    
    return currentNode;
}

/**
 * Get available next steps from the current position
 */
function getAvailableNextSteps() {
    const currentNode = getCurrentPathNode();
    if (!currentNode) return [];
    
    const nextSteps = [];
    currentNode.children.forEach((node, stepName) => {
        // Count how many tests are reachable from this step
        const testCount = countReachableTests(node);
        nextSteps.push({
            name: stepName,
            isFunction: node.isFunction,
            description: node.description,
            testCount: testCount,
            isEnd: node.isEnd
        });
    });
    
    return nextSteps.sort((a, b) => b.testCount - a.testCount);
}

/**
 * Count total reachable tests from a node (recursively)
 */
function countReachableTests(node) {
    let count = node.tests.length;
    node.children.forEach(child => {
        count += countReachableTests(child);
    });
    return count;
}

/**
 * Get tests that match the exact current path
 */
function getMatchingTests() {
    const currentNode = getCurrentPathNode();
    if (!currentNode) return [];
    return currentNode.tests || [];
}

function selectStep(stepName) {
    graphState.selectedPath.push(stepName);
    renderGraph();
}

function resetGraph() {
    graphState.selectedPath = [];
    graphState.matchingTests = [];
    renderGraph();
}

function goToStep(index) {
    // Navigate to a specific step in the path (remove everything after)
    graphState.selectedPath = graphState.selectedPath.slice(0, index);
    renderGraph();
}

function renderGraph() {
    const canvas = document.getElementById('graph-canvas');
    const emptyState = document.getElementById('graph-empty-state');
    const connections = document.getElementById('graph-connections');
    const centerNode = document.getElementById('graph-center-node');
    const breadcrumb = document.getElementById('graph-breadcrumb');
    
    if (!canvas || !connections) return;
    
    // Get available next steps
    const nextSteps = getAvailableNextSteps();
    const matchingTests = getMatchingTests();
    
    // Update state
    graphState.matchingTests = matchingTests;
    
    // Clear previous connections
    connections.innerHTML = '';
    
    // Always show canvas
    if (canvas) canvas.classList.add('active');
    if (emptyState) emptyState.classList.add('hidden');
    
    // Handle empty state - no steps available and no path selected
    if (nextSteps.length === 0 && graphState.selectedPath.length === 0) {
        if (emptyState) {
            emptyState.classList.remove('hidden');
            emptyState.innerHTML = `
                <span class="empty-icon">📊</span>
                <p>No simulation sequences found in test data</p>
            `;
        }
        if (canvas) canvas.classList.remove('active');
        if (centerNode) centerNode.classList.add('hidden');
        if (breadcrumb) breadcrumb.classList.add('hidden');
        return;
    }
    
    // Render the current path as breadcrumb
    renderPathBreadcrumb();
    
    // Render the center node (current position)
    if (centerNode) {
        if (graphState.selectedPath.length > 0) {
            const currentStep = graphState.selectedPath[graphState.selectedPath.length - 1];
            const currentNode = getCurrentPathNode();
            centerNode.textContent = currentStep;
            centerNode.classList.remove('hidden');
            centerNode.className = `graph-node center-node ${currentNode && currentNode.isFunction ? 'function-node' : 'logic-class-node'}`;
        } else {
            centerNode.classList.add('hidden');
        }
    }
    
    // Render next steps and matching tests
    renderNextStepsAndTests(connections, nextSteps, matchingTests);
}

function renderPathBreadcrumb() {
    const breadcrumb = document.getElementById('graph-breadcrumb');
    const trail = document.getElementById('breadcrumb-trail');
    
    if (!breadcrumb || !trail) return;
    
    if (graphState.selectedPath.length === 0) {
        breadcrumb.classList.add('hidden');
        return;
    }
    
    breadcrumb.classList.remove('hidden');
    trail.innerHTML = '';
    
    // Add "Start" at the beginning
    const startCrumb = document.createElement('span');
    startCrumb.className = 'breadcrumb-item breadcrumb-start';
    startCrumb.textContent = '🏁 Start';
    startCrumb.addEventListener('click', () => goToStep(0));
    trail.appendChild(startCrumb);
    
    // Build path to each step
    let pathNode = graphState.pathTree;
    graphState.selectedPath.forEach((stepName, index) => {
        // Add separator
        const separator = document.createElement('span');
        separator.className = 'breadcrumb-separator';
        separator.textContent = '→';
        trail.appendChild(separator);
        
        // Get node info
        pathNode = pathNode.children.get(stepName);
        const isFunction = pathNode ? pathNode.isFunction : false;
        
        // Add breadcrumb item
        const crumb = document.createElement('span');
        crumb.className = `breadcrumb-item ${isFunction ? 'function-type' : 'logic-class-type'}`;
        crumb.textContent = stepName;
        crumb.addEventListener('click', () => goToStep(index + 1));
        trail.appendChild(crumb);
    });
}

function renderNextStepsAndTests(container, nextSteps, matchingTests) {
    // Create wrapper for next steps
    if (nextSteps.length > 0) {
        const stackContainer = document.createElement('div');
        stackContainer.className = 'graph-connection-group right';
        
        const nodeStack = document.createElement('div');
        nodeStack.className = 'graph-node-stack';
        
        // Add label
        const label = document.createElement('div');
        label.className = 'graph-node-stack-label';
        label.textContent = `${nextSteps.length} next step${nextSteps.length === 1 ? '' : 's'} (click to select)`;
        nodeStack.appendChild(label);
        
        // Add each possible next step
        nextSteps.forEach(step => {
            const nodeEl = document.createElement('div');
            nodeEl.className = `graph-node clickable-step ${step.isFunction ? 'function-node' : 'logic-class-node'}`;
            nodeEl.textContent = step.name;
            
            // Add test count badge
            const badge = document.createElement('span');
            badge.className = 'node-test-count';
            badge.textContent = step.testCount;
            badge.title = `${step.testCount} test(s) reachable from this step`;
            nodeEl.appendChild(badge);
            
            // If this step is an endpoint, add indicator
            if (step.isEnd) {
                const endIndicator = document.createElement('span');
                endIndicator.className = 'step-end-indicator';
                endIndicator.textContent = '✓';
                endIndicator.title = 'This step completes a test sequence';
                nodeEl.appendChild(endIndicator);
            }
            
            nodeEl.addEventListener('click', () => selectStep(step.name));
            nodeStack.appendChild(nodeEl);
        });
        
        stackContainer.appendChild(nodeStack);
        container.appendChild(stackContainer);
        
        // Add connection line
        if (graphState.selectedPath.length > 0) {
            renderConnectionLines(container, nextSteps.length);
        }
    }
    
    // Show matching tests if we have them
    if (matchingTests.length > 0) {
        const testContainer = document.createElement('div');
        testContainer.className = 'graph-matching-tests';
        
        const testHeader = document.createElement('div');
        testHeader.className = 'matching-tests-header';
        testHeader.innerHTML = `<span class="tests-icon">✅</span> ${matchingTests.length} test${matchingTests.length === 1 ? '' : 's'} available at this path:`;
        testContainer.appendChild(testHeader);
        
        const testList = document.createElement('div');
        testList.className = 'matching-tests-list';
        
        matchingTests.forEach(testName => {
            const testItem = document.createElement('div');
            testItem.className = 'matching-test-item';
            testItem.textContent = testName;
            testItem.addEventListener('click', () => {
                // Find and show test details
                const test = TEST_DATA.find(t => t.name === testName);
                if (test) showDetails(test);
            });
            testList.appendChild(testItem);
        });
        
        testContainer.appendChild(testList);
        container.appendChild(testContainer);
    }
    
    // If no next steps and no matching tests at root, show instruction
    if (nextSteps.length === 0 && matchingTests.length === 0 && graphState.selectedPath.length === 0) {
        const msg = document.createElement('div');
        msg.className = 'graph-no-connections';
        msg.innerHTML = '<p>No simulation paths available</p>';
        container.appendChild(msg);
    }
    
    // If path selected but dead end (no next steps, no matching tests at this exact point)
    if (nextSteps.length === 0 && matchingTests.length === 0 && graphState.selectedPath.length > 0) {
        const msg = document.createElement('div');
        msg.className = 'graph-end-of-path';
        msg.innerHTML = `
            <span class="end-icon">🏁</span>
            <p>End of path - no further steps available</p>
        `;
        container.appendChild(msg);
    }
}

function renderConnectionLines(container, nodeCount) {
    // Add a dashed connection line from center to the node stack
    const connectionLine = document.createElement('div');
    connectionLine.className = 'graph-connection-line';
    container.insertBefore(connectionLine, container.firstChild);
}
