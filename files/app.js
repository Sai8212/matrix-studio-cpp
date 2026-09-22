/**
 * MATRIX OPERATIONS APPLICATION - CORE LOGIC & LIQUID GLASS UI ENGINE
 * 
 * NOTE: Mathematical algorithms, dimension validation, and parsing rules
 * strictly follow Matrix.h and Matrix.cpp from the original university application.
 */

// ============================================================================
// 1. Matrix Class (Direct Mirror of Matrix.h / Matrix.cpp)
// ============================================================================

class Matrix {
    constructor(rows, cols) {
        this.rows = parseInt(rows, 10);
        this.cols = parseInt(cols, 10);
        this.data = Array.from({ length: this.rows }, () => Array(this.cols).fill(0));
    }

    getRows() { return this.rows; }
    getCols() { return this.cols; }
    get(i, j) { return this.data[i][j]; }
    set(i, j, value) { this.data[i][j] = parseInt(value, 10) || 0; }

    // ----- Matrix Addition -----
    // Rule: order(A) must equal order(B)
    add(other) {
        if (this.rows !== other.rows || this.cols !== other.cols) {
            throw new Error(
                `Addition error: matrices must have the same order. ` +
                `Matrix A is ${this.rows}x${this.cols}, ` +
                `Matrix B is ${other.rows}x${other.cols}.`
            );
        }
        const result = new Matrix(this.rows, this.cols);
        for (let i = 0; i < this.rows; ++i) {
            for (let j = 0; j < this.cols; ++j) {
                result.data[i][j] = this.data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    // ----- Matrix Multiplication -----
    // Rule: cols(A) must equal rows(B)
    multiply(other) {
        if (this.cols !== other.rows) {
            throw new Error(
                `Multiplication error: columns of Matrix A (${this.cols}) ` +
                `must equal rows of Matrix B (${other.rows}).`
            );
        }
        const result = new Matrix(this.rows, other.cols);
        for (let i = 0; i < this.rows; ++i) {
            for (let j = 0; j < other.cols; ++j) {
                for (let k = 0; k < this.cols; ++k) {
                    result.data[i][j] += this.data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    // ----- Matrix Subtraction -----
    // Rule: order(A) must equal order(B)
    subtract(other) {
        if (this.rows !== other.rows || this.cols !== other.cols) {
            throw new Error(
                `Subtraction error: matrices must have the same order. ` +
                `Matrix A is ${this.rows}x${this.cols}, ` +
                `Matrix B is ${other.rows}x${other.cols}.`
            );
        }
        const result = new Matrix(this.rows, this.cols);
        for (let i = 0; i < this.rows; ++i) {
            for (let j = 0; j < this.cols; ++j) {
                result.data[i][j] = this.data[i][j] - other.data[i][j];
            }
        }
        return result;
    }

    // ----- Determinant (Recursive Cofactor Expansion) -----
    // Rule: Matrix must be square (n x n)
    determinant() {
        if (this.rows !== this.cols) {
            throw new Error(
                `Determinant error: matrix must be square. ` +
                `This matrix is ${this.rows}x${this.cols}.`
            );
        }
        return this._det(this.data);
    }

    _det(mat) {
        const n = mat.length;
        if (n === 1) return mat[0][0];
        if (n === 2) return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
        let det = 0;
        for (let j = 0; j < n; j++) {
            const minor = mat
                .slice(1)
                .map(row => row.filter((_, col) => col !== j));
            det += Math.pow(-1, j) * mat[0][j] * this._det(minor);
        }
        return det;
    }

    // ----- Inverse Matrix (Adjugate / Classical Adjoint Method) -----
    // Rule: Matrix must be square AND det(A) != 0
    inverse() {
        if (this.rows !== this.cols) {
            throw new Error(
                `Inverse error: matrix must be square. ` +
                `This matrix is ${this.rows}x${this.cols}.`
            );
        }
        const det = this.determinant();
        if (det === 0) {
            throw new Error(
                `Inverse error: matrix is singular (det = 0). ` +
                `The inverse does not exist.`
            );
        }
        const n = this.rows;
        // Build cofactor matrix
        const cof = Array.from({ length: n }, () => Array(n).fill(0));
        for (let i = 0; i < n; i++) {
            for (let j = 0; j < n; j++) {
                const minor = this.data
                    .filter((_, r) => r !== i)
                    .map(row => row.filter((_, c) => c !== j));
                cof[i][j] = Math.pow(-1, i + j) * this._det(minor);
            }
        }
        // Transpose cofactor to get adjugate, then divide by det
        const result = new Matrix(n, n);
        for (let i = 0; i < n; i++) {
            for (let j = 0; j < n; j++) {
                // Store as fraction-like float rounded to 6 decimals
                result.data[i][j] = Math.round((cof[j][i] / det) * 1e6) / 1e6;
            }
        }
        return result;
    }

    // ----- Matrix Transpose -----
    // No dimension restriction: works on any matrix
    transpose() {
        const result = new Matrix(this.cols, this.rows);
        for (let i = 0; i < this.rows; ++i) {
            for (let j = 0; j < this.cols; ++j) {
                result.set(j, i, this.data[i][j]);
            }
        }
        return result;
    }

    // Formats the matrix as text with CRLF line endings, ready to
    // drop straight into a Windows edit control (exact match to Matrix::toString).
    toString() {
        let out = "";
        for (let i = 0; i < this.rows; ++i) {
            for (let j = 0; j < this.cols; ++j) {
                out += this.data[i][j];
                if (j !== this.cols - 1) out += "\t";
            }
            out += "\r\n";
        }
        return out;
    }

    // Formats into the input syntax:
    // rows cols
    // e1 e2 ...
    toInputString() {
        let out = `${this.rows} ${this.cols}\r\n`;
        for (let i = 0; i < this.rows; ++i) {
            out += this.data[i].join(" ");
            if (i !== this.rows - 1) out += "\r\n";
        }
        return out;
    }
}

// Parses text typed into a text box, shaped like:
//   rows cols
//   e1 e2 e3 ...
// Throws Error matching the exact C++ std::invalid_argument message
function parseMatrixFromText(text, label) {
    if (!text || typeof text !== "string") {
        throw new Error(`Matrix ${label}: the first line must contain two whole numbers - rows and columns.`);
    }

    // Split text into tokens by whitespace
    const tokens = text.trim().split(/\s+/).filter(Boolean);
    if (tokens.length < 2) {
        throw new Error(`Matrix ${label}: the first line must contain two whole numbers - rows and columns.`);
    }

    const r = parseInt(tokens[0], 10);
    const c = parseInt(tokens[1], 10);

    if (isNaN(r) || isNaN(c)) {
        throw new Error(`Matrix ${label}: the first line must contain two whole numbers - rows and columns.`);
    }
    if (r <= 0 || c <= 0) {
        throw new Error(`Matrix ${label}: rows and columns must be positive integers.`);
    }

    const expectedCount = r * c;
    const elementTokens = tokens.slice(2);

    if (elementTokens.length < expectedCount) {
        throw new Error(
            `Matrix ${label}: expected ${expectedCount} elements (a ${r}x${c} matrix) ` +
            `but not enough numbers were entered.`
        );
    }

    const m = new Matrix(r, c);
    let tokenIdx = 0;
    for (let i = 0; i < r; ++i) {
        for (let j = 0; j < c; ++j) {
            const val = parseInt(elementTokens[tokenIdx++], 10);
            if (isNaN(val)) {
                throw new Error(
                    `Matrix ${label}: expected ${expectedCount} elements (a ${r}x${c} matrix) ` +
                    `but not enough numbers were entered.`
                );
            }
            m.set(i, j, val);
        }
    }
    return m;
}

// ============================================================================
// 2. Application State & Controller
// ============================================================================

const AppState = {
    currentPage: 'home',
    theme: 'dark',
    viewMode: 'grid', // 'grid' or 'raw'
    glassBlur: 'medium',
    enableAnimations: true,

    matrixA: new Matrix(2, 2),
    matrixB: new Matrix(2, 2),
    lastResultMatrix: null,
    lastResultTitle: '',
    lastResultSteps: '',

    history: []
};

// Initialize matrices with university default values:
// A: 2x2 [1, 2], [3, 4]
AppState.matrixA.set(0, 0, 1);
AppState.matrixA.set(0, 1, 2);
AppState.matrixA.set(1, 0, 3);
AppState.matrixA.set(1, 1, 4);

// B: 2x2 [5, 6], [7, 8]
AppState.matrixB.set(0, 0, 5);
AppState.matrixB.set(0, 1, 6);
AppState.matrixB.set(1, 0, 7);
AppState.matrixB.set(1, 1, 8);

// ============================================================================
// 3. UI Rendering & Matrix Grid Builders
// ============================================================================

function renderMatrixGrid(matrixId, matrix) {
    const container = document.getElementById(`grid-cells-${matrixId}`);
    if (!container) return;

    container.innerHTML = '';
    container.style.gridTemplateColumns = `repeat(${matrix.getCols()}, 54px)`;

    for (let i = 0; i < matrix.getRows(); ++i) {
        for (let j = 0; j < matrix.getCols(); ++j) {
            const input = document.createElement('input');
            input.type = 'number';
            input.className = 'matrix-cell-input';
            input.value = matrix.get(i, j);
            input.dataset.row = i;
            input.dataset.col = j;
            input.dataset.matrix = matrixId;
            input.setAttribute('aria-label', `Matrix ${matrixId.toUpperCase()} [${i+1},${j+1}]`);

            // Cell update listener
            input.addEventListener('input', (e) => {
                const val = parseInt(e.target.value, 10) || 0;
                matrix.set(i, j, val);
                syncMatrixToRawText(matrixId, matrix);
            });

            // Keyboard navigation
            input.addEventListener('keydown', (e) => handleCellKeyNavigation(e, matrixId, i, j));

            container.appendChild(input);
        }
    }

    // Update order badge
    const badge = document.getElementById(`order-badge-${matrixId}`);
    if (badge) badge.textContent = `${matrix.getRows()} × ${matrix.getCols()}`;

    // Update stepper numbers
    const rowsSpan = document.getElementById(`rows-val-${matrixId}`);
    const colsSpan = document.getElementById(`cols-val-${matrixId}`);
    if (rowsSpan) rowsSpan.textContent = matrix.getRows();
    if (colsSpan) colsSpan.textContent = matrix.getCols();

    // Update preset pills active state
    updatePresetPillState(matrixId, matrix.getRows(), matrix.getCols());

    // Sync raw textarea
    syncMatrixToRawText(matrixId, matrix);
}

function handleCellKeyNavigation(e, matrixId, row, col) {
    const matrix = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
    const rCount = matrix.getRows();
    const cCount = matrix.getCols();

    let targetRow = row;
    let targetCol = col;

    if (e.key === 'ArrowRight') {
        if (col + 1 < cCount) targetCol++;
        else if (row + 1 < rCount) { targetRow++; targetCol = 0; }
    } else if (e.key === 'ArrowLeft') {
        if (col - 1 >= 0) targetCol--;
        else if (row - 1 >= 0) { targetRow--; targetCol = cCount - 1; }
    } else if (e.key === 'ArrowDown') {
        if (row + 1 < rCount) targetRow++;
    } else if (e.key === 'ArrowUp') {
        if (row - 1 >= 0) targetRow--;
    } else if (e.key === 'Enter') {
        e.preventDefault();
        if (col + 1 < cCount) targetCol++;
        else if (row + 1 < rCount) { targetRow++; targetCol = 0; }
    } else {
        return; // Allow standard input
    }

    if (targetRow !== row || targetCol !== col) {
        e.preventDefault();
        const container = document.getElementById(`grid-cells-${matrixId}`);
        const inputs = container.querySelectorAll('.matrix-cell-input');
        const targetIndex = targetRow * cCount + targetCol;
        if (inputs[targetIndex]) {
            inputs[targetIndex].focus();
            inputs[targetIndex].select();
        }
    }
}

function syncMatrixToRawText(matrixId, matrix) {
    const textarea = document.getElementById(`raw-textarea-${matrixId}`);
    if (textarea && document.activeElement !== textarea) {
        textarea.value = matrix.toInputString();
    }
}

function syncRawTextToMatrix(matrixId) {
    const textarea = document.getElementById(`raw-textarea-${matrixId}`);
    if (!textarea) return;

    try {
        const parsed = parseMatrixFromText(textarea.value, matrixId.toUpperCase());
        if (matrixId === 'a') {
            AppState.matrixA = parsed;
            renderMatrixGrid('a', AppState.matrixA);
        } else {
            AppState.matrixB = parsed;
            renderMatrixGrid('b', AppState.matrixB);
        }
        hideError();
    } catch (err) {
        showError(err.message);
    }
}

function changeMatrixDimensions(matrixId, newRows, newCols) {
    newRows = Math.max(1, Math.min(6, newRows));
    newCols = Math.max(1, Math.min(6, newCols));

    const oldMatrix = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
    const newMatrix = new Matrix(newRows, newCols);

    // Preserve existing values where dimensions overlap
    for (let i = 0; i < Math.min(oldMatrix.getRows(), newRows); i++) {
        for (let j = 0; j < Math.min(oldMatrix.getCols(), newCols); j++) {
            newMatrix.set(i, j, oldMatrix.get(i, j));
        }
    }

    if (matrixId === 'a') {
        AppState.matrixA = newMatrix;
        renderMatrixGrid('a', AppState.matrixA);
    } else {
        AppState.matrixB = newMatrix;
        renderMatrixGrid('b', AppState.matrixB);
    }
}

function updatePresetPillState(matrixId, rows, cols) {
    const pills = document.querySelectorAll(`[data-preset-for="${matrixId}"]`);
    pills.forEach(pill => {
        const pR = parseInt(pill.dataset.rows, 10);
        const pC = parseInt(pill.dataset.cols, 10);
        if (pR === rows && pC === cols) pill.classList.add('active');
        else pill.classList.remove('active');
    });
}

// Quick Preset Matrix Fillers
function fillMatrixPreset(matrixId, type) {
    const matrix = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
    const r = matrix.getRows();
    const c = matrix.getCols();

    for (let i = 0; i < r; i++) {
        for (let j = 0; j < c; j++) {
            if (type === 'zero') {
                matrix.set(i, j, 0);
            } else if (type === 'identity') {
                matrix.set(i, j, i === j ? 1 : 0);
            } else if (type === 'random') {
                matrix.set(i, j, Math.floor(Math.random() * 19) - 9); // -9 to 9
            }
        }
    }
    renderMatrixGrid(matrixId, matrix);
}

// ============================================================================
// 4. Matrix Calculations & Execution
// ============================================================================

function executeAddition() {
    try {
        hideError();
        // Ensure raw text edits are captured if raw mode was active
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
            AppState.matrixB = parseMatrixFromText(document.getElementById('raw-textarea-b').value, 'B');
        }

        const a = AppState.matrixA;
        const b = AppState.matrixB;
        const result = a.add(b);

        // Generate Step-by-Step Breakdown
        let steps = `Addition Formula: C[i][j] = A[i][j] + B[i][j]\r\n`;
        steps += `Dimensions: ${a.getRows()}x${a.getCols()} + ${b.getRows()}x${b.getCols()} = ${result.getRows()}x${result.getCols()}\r\n\r\n`;
        for (let i = 0; i < a.getRows(); i++) {
            for (let j = 0; j < a.getCols(); j++) {
                steps += `C[${i+1},${j+1}] = (${a.get(i, j)}) + (${b.get(i, j)}) = ${result.get(i, j)}\r\n`;
            }
        }

        displayResult(result, "Addition (A + B)", steps);
    } catch (err) {
        showError(err.message);
    }
}

function executeMultiplication() {
    try {
        hideError();
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
            AppState.matrixB = parseMatrixFromText(document.getElementById('raw-textarea-b').value, 'B');
        }

        const a = AppState.matrixA;
        const b = AppState.matrixB;
        const result = a.multiply(b);

        // Generate Step-by-Step Breakdown
        let steps = `Multiplication Formula: C[i][j] = Σ (A[i][k] * B[k][j]) for k=1..${a.getCols()}\r\n`;
        steps += `Dimensions: ${a.getRows()}x${a.getCols()} * ${b.getRows()}x${b.getCols()} = ${result.getRows()}x${result.getCols()}\r\n\r\n`;
        for (let i = 0; i < a.getRows(); i++) {
            for (let j = 0; j < b.getCols(); j++) {
                const termStrs = [];
                for (let k = 0; k < a.getCols(); k++) {
                    termStrs.push(`(${a.get(i, k)} * ${b.get(k, j)})`);
                }
                steps += `C[${i+1},${j+1}] = ${termStrs.join(" + ")} = ${result.get(i, j)}\r\n`;
            }
        }

        displayResult(result, "Multiplication (A × B)", steps);
    } catch (err) {
        showError(err.message);
    }
}

function executeTranspose() {
    try {
        hideError();
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
        }

        const a = AppState.matrixA;
        const result = a.transpose();

        let steps = `Transpose Formula: C[j][i] = A[i][j] (swapping rows and columns)\r\n`;
        steps += `Original Dimensions: ${a.getRows()}x${a.getCols()} → Transposed: ${result.getRows()}x${result.getCols()}\r\n\r\n`;
        for (let i = 0; i < a.getRows(); i++) {
            for (let j = 0; j < a.getCols(); j++) {
                steps += `A[${i+1},${j+1}] (${a.get(i, j)}) → C[${j+1},${i+1}]\r\n`;
            }
        }

        displayResult(result, "Transpose of A", steps);
    } catch (err) {
        showError(err.message);
    }
}

function executeSubtraction() {
    try {
        hideError();
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
            AppState.matrixB = parseMatrixFromText(document.getElementById('raw-textarea-b').value, 'B');
        }

        const a = AppState.matrixA;
        const b = AppState.matrixB;
        const result = a.subtract(b);

        let steps = `Subtraction Formula: C[i][j] = A[i][j] - B[i][j]\r\n`;
        steps += `Dimensions: ${a.getRows()}x${a.getCols()} - ${b.getRows()}x${b.getCols()} = ${result.getRows()}x${result.getCols()}\r\n\r\n`;
        for (let i = 0; i < a.getRows(); i++) {
            for (let j = 0; j < a.getCols(); j++) {
                steps += `C[${i+1},${j+1}] = (${a.get(i, j)}) - (${b.get(i, j)}) = ${result.get(i, j)}\r\n`;
            }
        }

        displayResult(result, "Subtraction (A - B)", steps);
    } catch (err) {
        showError(err.message);
    }
}

function executeDeterminant() {
    try {
        hideError();
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
        }

        const a = AppState.matrixA;
        const det = a.determinant();
        const n = a.getRows();

        // Display determinant as a 1x1 matrix result
        const scalarResult = new Matrix(1, 1);
        scalarResult.data[0][0] = det;

        let steps = `Determinant of A (${n}x${n} matrix):\r\n`;
        steps += `Method: Cofactor Expansion along Row 1\r\n\r\n`;
        if (n === 1) {
            steps += `det(A) = ${a.get(0, 0)}\r\n`;
        } else if (n === 2) {
            steps += `det(A) = A[1,1]*A[2,2] - A[1,2]*A[2,1]\r\n`;
            steps += `       = (${a.get(0,0)})*(${a.get(1,1)}) - (${a.get(0,1)})*(${a.get(1,0)})\r\n`;
            steps += `       = ${a.get(0,0)*a.get(1,1)} - ${a.get(0,1)*a.get(1,0)}\r\n`;
            steps += `       = ${det}\r\n`;
        } else {
            steps += `Expanding along Row 1:\r\n`;
            for (let j = 0; j < n; j++) {
                const sign = Math.pow(-1, j) >= 0 ? '+' : '-';
                steps += `  ${sign} A[1,${j+1}] * M[1,${j+1}]  (element = ${a.get(0, j)})\r\n`;
            }
            steps += `\r\nFinal Result: det(A) = ${det}\r\n`;
        }
        steps += `\r\nNote: det(A) = ${det === 0 ? '0 → Matrix is SINGULAR (no inverse exists)' : det + ' → Matrix is NON-SINGULAR (inverse exists)'}`;

        displayResult(scalarResult, `Determinant of A = ${det}`, steps);
    } catch (err) {
        showError(err.message);
    }
}

function executeInverse() {
    try {
        hideError();
        if (AppState.viewMode === 'raw') {
            AppState.matrixA = parseMatrixFromText(document.getElementById('raw-textarea-a').value, 'A');
        }

        const a = AppState.matrixA;
        const det = a.determinant();
        const result = a.inverse();
        const n = a.getRows();

        let steps = `Inverse of A (${n}x${n} matrix):\r\n`;
        steps += `Method: Classical Adjoint (Adjugate) Method\r\n\r\n`;
        steps += `Step 1: Compute det(A) = ${det}\r\n`;
        steps += `Step 2: Compute cofactor matrix C[i][j] = (-1)^(i+j) * M[i][j]\r\n`;
        steps += `Step 3: Transpose cofactor matrix to get Adjugate\r\n`;
        steps += `Step 4: A⁻¹ = Adjugate(A) / det(A)\r\n\r\n`;
        steps += `Result: A⁻¹ (values shown rounded to 6 decimal places):\r\n`;
        for (let i = 0; i < n; i++) {
            const row = [];
            for (let j = 0; j < n; j++) {
                row.push(`A⁻¹[${i+1},${j+1}] = ${result.get(i, j)}`);
            }
            steps += row.join('  |  ') + `\r\n`;
        }

        displayResult(result, `Inverse of A (det=${det})`, steps);
    } catch (err) {
        showError(err.message);
    }
}

function displayResult(resultMatrix, operationTitle, steps) {
    AppState.lastResultMatrix = resultMatrix;
    AppState.lastResultTitle = operationTitle;
    AppState.lastResultSteps = steps;

    // Hide empty placeholder, show content
    const emptyState = document.getElementById('result-empty-state');
    const displayArea = document.getElementById('result-active-display');
    if (emptyState) emptyState.style.display = 'none';
    if (displayArea) displayArea.style.display = 'flex';

    // Set Operation Badge
    const opTag = document.getElementById('result-op-tag');
    if (opTag) opTag.textContent = `${operationTitle} (${resultMatrix.getRows()} × ${resultMatrix.getCols()})`;

    // Build the Visual Bracketed Cells Grid
    const cellsGrid = document.getElementById('result-cells-grid');
    if (cellsGrid) {
        cellsGrid.innerHTML = '';
        cellsGrid.style.gridTemplateColumns = `repeat(${resultMatrix.getCols()}, minmax(58px, 1fr))`;

        for (let i = 0; i < resultMatrix.getRows(); i++) {
            for (let j = 0; j < resultMatrix.getCols(); j++) {
                const cell = document.createElement('div');
                cell.className = 'result-cell-pill';
                cell.textContent = resultMatrix.get(i, j);
                cellsGrid.appendChild(cell);
            }
        }
    }

    // Set Raw Text box (matching C++ Matrix::toString format)
    const rawBox = document.getElementById('result-raw-output');
    if (rawBox) {
        rawBox.textContent = `Result (${operationTitle}):\r\n\r\n` + resultMatrix.toString();
    }

    // Set Step-by-Step Breakdown
    const stepsBody = document.getElementById('result-steps-body');
    if (stepsBody) {
        stepsBody.textContent = steps;
    }

    // Record into History
    AppState.history.unshift({
        title: operationTitle,
        matrix: resultMatrix,
        time: new Date().toLocaleTimeString()
    });
}

// Error Banner Handling
function showError(message) {
    const banner = document.getElementById('glass-error-banner');
    const text = document.getElementById('error-message-text');
    if (banner && text) {
        text.textContent = message;
        banner.classList.add('active');
        banner.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
    }
}

function hideError() {
    const banner = document.getElementById('glass-error-banner');
    if (banner) banner.classList.remove('active');
}

// ============================================================================
// 5. Result Actions (Copying, Chaining)
// ============================================================================

function copyResultToClipboard() {
    if (!AppState.lastResultMatrix) return;
    const rawText = AppState.lastResultMatrix.toString();
    navigator.clipboard.writeText(rawText).then(() => {
        const btn = document.getElementById('btn-copy-result');
        if (btn) {
            const originalHtml = btn.innerHTML;
            btn.innerHTML = `<span class="btn-symbol">✓</span> Copied`;
            setTimeout(() => { btn.innerHTML = originalHtml; }, 1800);
        }
    }).catch(() => {
        // Fallback
        const rawBox = document.getElementById('result-raw-output');
        if (rawBox) {
            const range = document.createRange();
            range.selectNodeContents(rawBox);
            window.getSelection().removeAllRanges();
            window.getSelection().addRange(range);
        }
    });
}

function copyResultToMatrixA() {
    if (!AppState.lastResultMatrix) return;
    AppState.matrixA = AppState.lastResultMatrix;
    renderMatrixGrid('a', AppState.matrixA);
    const btn = document.getElementById('btn-use-as-a');
    if (btn) {
        const originalHtml = btn.innerHTML;
        btn.innerHTML = `<span class="btn-symbol">✓</span> Applied to Matrix A`;
        setTimeout(() => { btn.innerHTML = originalHtml; }, 1800);
    }
}

// ============================================================================
// 6. Navigation & Page Controller
// ============================================================================

function navigateToPage(pageId) {
    AppState.currentPage = pageId;

    // Update Nav buttons active state
    document.querySelectorAll('.nav-item-btn').forEach(btn => {
        if (btn.dataset.page === pageId) btn.classList.add('active');
        else btn.classList.remove('active');
    });

    // Update page views visibility
    document.querySelectorAll('.page-view').forEach(view => {
        if (view.id === `page-${pageId}`) view.classList.add('active');
        else view.classList.remove('active');
    });

    // Update Header Breadcrumbs & Title
    const headerTitle = document.getElementById('header-title');
    const headerBreadcrumb = document.getElementById('header-breadcrumb');

    const pageMeta = {
        home: { title: "Matrix Studio", crumb: "System / Overview" },
        basics: { title: "Matrix Basics", crumb: "Learning / Foundations" },
        types: { title: "Types of Matrices", crumb: "Reference / Taxonomy" },
        operations: { title: "Matrix Operations", crumb: "Reference / Rules" },
        calculator: { title: "Matrix Calculator", crumb: "Workspace / Engine" },
        settings: { title: "Preferences & Appearance", crumb: "System / Settings" },
        about: { title: "About Application", crumb: "System / Architecture" }
    };

    if (pageMeta[pageId]) {
        if (headerTitle) headerTitle.textContent = pageMeta[pageId].title;
        if (headerBreadcrumb) headerBreadcrumb.textContent = pageMeta[pageId].crumb;
    }

    // Scroll to top of container
    const container = document.querySelector('.page-container');
    if (container) container.scrollTop = 0;
}

// ============================================================================
// 7. Theme & Customization Settings
// ============================================================================

function applyTheme(themeName) {
    AppState.theme = themeName;
    document.body.classList.remove('theme-light', 'theme-cyber');
    if (themeName === 'light') document.body.classList.add('theme-light');
    if (themeName === 'cyber') document.body.classList.add('theme-cyber');

    // Update Settings UI active cards
    document.querySelectorAll('.theme-card-option').forEach(card => {
        if (card.dataset.theme === themeName) card.classList.add('active');
        else card.classList.remove('active');
    });
}

function setGlassBlur(intensity) {
    AppState.glassBlur = intensity;
    document.body.classList.remove('blur-low', 'blur-medium', 'blur-ultra');
    document.body.classList.add(`blur-${intensity}`);

    document.querySelectorAll('.intensity-pill').forEach(pill => {
        if (pill.dataset.intensity === intensity) pill.classList.add('active');
        else pill.classList.remove('active');
    });
}

function toggleAnimations(enable) {
    AppState.enableAnimations = enable;
    if (!enable) document.body.classList.add('no-animation');
    else document.body.classList.remove('no-animation');
}

// ============================================================================
// 8. Event Listeners & Bootstrapping
// ============================================================================

document.addEventListener('DOMContentLoaded', () => {
    // Initial renders
    renderMatrixGrid('a', AppState.matrixA);
    renderMatrixGrid('b', AppState.matrixB);

    // Navigation item clicks
    document.querySelectorAll('.nav-item-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            navigateToPage(btn.dataset.page);
        });
    });

    // Quick Action Launchers from Home page
    document.querySelectorAll('[data-launch-calc]').forEach(el => {
        el.addEventListener('click', () => {
            navigateToPage('calculator');
            const op = el.dataset.launchCalc;
            if (op === 'add') executeAddition();
            else if (op === 'sub') executeSubtraction();
            else if (op === 'mul') executeMultiplication();
            else if (op === 'trans') executeTranspose();
            else if (op === 'det') executeDeterminant();
            else if (op === 'inv') executeInverse();
        });
    });

    // Calculator View Mode Switch (Grid vs Raw)
    const btnModeGrid = document.getElementById('btn-mode-grid');
    const btnModeRaw = document.getElementById('btn-mode-raw');
    if (btnModeGrid && btnModeRaw) {
        btnModeGrid.addEventListener('click', () => {
            AppState.viewMode = 'grid';
            btnModeGrid.classList.add('active');
            btnModeRaw.classList.remove('active');
            document.querySelectorAll('.matrix-bracket-stage').forEach(el => el.style.display = 'flex');
            document.querySelectorAll('.matrix-raw-mode').forEach(el => el.classList.remove('active'));
        });

        btnModeRaw.addEventListener('click', () => {
            AppState.viewMode = 'raw';
            btnModeRaw.classList.add('active');
            btnModeGrid.classList.remove('active');
            document.querySelectorAll('.matrix-bracket-stage').forEach(el => el.style.display = 'none');
            document.querySelectorAll('.matrix-raw-mode').forEach(el => el.classList.add('active'));
        });
    }

    // Raw Textarea Input Listeners
    const rawA = document.getElementById('raw-textarea-a');
    if (rawA) rawA.addEventListener('input', () => syncRawTextToMatrix('a'));
    const rawB = document.getElementById('raw-textarea-b');
    if (rawB) rawB.addEventListener('input', () => syncRawTextToMatrix('b'));

    // Dimension Steppers - Matrix A
    setupDimensionSteppers('a');
    setupDimensionSteppers('b');

    // Preset Dimension Pills
    document.querySelectorAll('.preset-dim-pill').forEach(pill => {
        pill.addEventListener('click', () => {
            const matrixId = pill.dataset.presetFor;
            const r = parseInt(pill.dataset.rows, 10);
            const c = parseInt(pill.dataset.cols, 10);
            changeMatrixDimensions(matrixId, r, c);
        });
    });

    // Matrix Quick Actions (Zero, Identity, Random, Clear)
    document.querySelectorAll('.matrix-quick-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            const matrixId = btn.dataset.matrix;
            const action = btn.dataset.action;
            fillMatrixPreset(matrixId, action);
        });
    });

    // Operation Buttons
    const btnAdd = document.getElementById('calc-btn-add');
    if (btnAdd) btnAdd.addEventListener('click', executeAddition);
    const btnSub = document.getElementById('calc-btn-sub');
    if (btnSub) btnSub.addEventListener('click', executeSubtraction);
    const btnMul = document.getElementById('calc-btn-mul');
    if (btnMul) btnMul.addEventListener('click', executeMultiplication);
    const btnTrans = document.getElementById('calc-btn-trans');
    if (btnTrans) btnTrans.addEventListener('click', executeTranspose);
    const btnDet = document.getElementById('calc-btn-det');
    if (btnDet) btnDet.addEventListener('click', executeDeterminant);
    const btnInv = document.getElementById('calc-btn-inv');
    if (btnInv) btnInv.addEventListener('click', executeInverse);

    // Error Banner Close
    const btnErrorClose = document.getElementById('error-close-btn');
    if (btnErrorClose) btnErrorClose.addEventListener('click', hideError);

    // Result Actions
    const btnCopy = document.getElementById('btn-copy-result');
    if (btnCopy) btnCopy.addEventListener('click', copyResultToClipboard);
    const btnUseAsA = document.getElementById('btn-use-as-a');
    if (btnUseAsA) btnUseAsA.addEventListener('click', copyResultToMatrixA);

    // Quick Theme Toggle in Header
    const btnHeaderTheme = document.getElementById('header-theme-toggle');
    if (btnHeaderTheme) {
        btnHeaderTheme.addEventListener('click', () => {
            const nextTheme = AppState.theme === 'dark' ? 'light' : (AppState.theme === 'light' ? 'cyber' : 'dark');
            applyTheme(nextTheme);
        });
    }

    // Theme Selector in Settings
    document.querySelectorAll('.theme-card-option').forEach(card => {
        card.addEventListener('click', () => {
            applyTheme(card.dataset.theme);
        });
    });

    // Glass Intensity in Settings
    document.querySelectorAll('.intensity-pill').forEach(pill => {
        pill.addEventListener('click', () => {
            setGlassBlur(pill.dataset.intensity);
        });
    });

    // Animation Switch in Settings
    const animToggle = document.getElementById('setting-anim-toggle');
    if (animToggle) {
        animToggle.addEventListener('change', (e) => {
            toggleAnimations(e.target.checked);
        });
    }

    // Educational Interactive Diagram in Basics Page
    document.querySelectorAll('.concept-cell').forEach(cell => {
        cell.addEventListener('mouseenter', () => {
            const row = cell.dataset.row;
            const col = cell.dataset.col;
            const callout = document.getElementById('basics-concept-callout');
            if (callout) {
                callout.innerHTML = `Element at <strong>Row ${row}</strong>, <strong>Column ${col}</strong>: written as <strong>A[${row}][${col}]</strong> with value <strong>${cell.querySelector('.concept-cell-val').textContent}</strong>.`;
            }
        });
    });

    // Types Taxonomy Filter Pills
    document.querySelectorAll('.category-filter-pill').forEach(pill => {
        pill.addEventListener('click', () => {
            document.querySelectorAll('.category-filter-pill').forEach(p => p.classList.remove('active'));
            pill.classList.add('active');
            const cat = pill.dataset.category;
            document.querySelectorAll('.type-matrix-card').forEach(card => {
                if (cat === 'all' || card.dataset.category === cat) {
                    card.style.display = 'flex';
                } else {
                    card.style.display = 'none';
                }
            });
        });
    });

    // Default start on Home
    navigateToPage('home');
});

function setupDimensionSteppers(matrixId) {
    const btnRowMinus = document.getElementById(`rows-minus-${matrixId}`);
    const btnRowPlus = document.getElementById(`rows-plus-${matrixId}`);
    const btnColMinus = document.getElementById(`cols-minus-${matrixId}`);
    const btnColPlus = document.getElementById(`cols-plus-${matrixId}`);

    if (btnRowMinus && btnRowPlus && btnColMinus && btnColPlus) {
        btnRowMinus.addEventListener('click', () => {
            const m = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
            changeMatrixDimensions(matrixId, m.getRows() - 1, m.getCols());
        });
        btnRowPlus.addEventListener('click', () => {
            const m = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
            changeMatrixDimensions(matrixId, m.getRows() + 1, m.getCols());
        });
        btnColMinus.addEventListener('click', () => {
            const m = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
            changeMatrixDimensions(matrixId, m.getRows(), m.getCols() - 1);
        });
        btnColPlus.addEventListener('click', () => {
            const m = matrixId === 'a' ? AppState.matrixA : AppState.matrixB;
            changeMatrixDimensions(matrixId, m.getRows(), m.getCols() + 1);
        });
    }
}
