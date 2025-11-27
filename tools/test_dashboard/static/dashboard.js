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

// Initialize dashboard
document.addEventListener('DOMContentLoaded', () => {
    initializeEventListeners();
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
