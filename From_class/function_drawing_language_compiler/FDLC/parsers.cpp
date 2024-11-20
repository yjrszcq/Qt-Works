#include "parsers.h"

int Parsers::cnt = 0;

Parsers::Parsers(QObject *parent)
    : QObject{parent}
{}

void Parsers::outExprNode(struct ExprNode* tree) {
    switch (tree->op_code) {
    case CONST_ID: emit parsersOutputSent("\t\t" + QString::number(tree->content.case_const), Qt::gray); break;
    case T: emit parsersOutputSent("\t\tT", Qt::gray); break;
    case FUNC: {
        QString funcName;
        if (tree->content.case_func.math_func_ptr == sin) { funcName = "sin"; }
        else if (tree->content.case_func.math_func_ptr == cos) { funcName = "cos"; }
        else if (tree->content.case_func.math_func_ptr == log) { funcName = "log"; }
        else if (tree->content.case_func.math_func_ptr == exp) { funcName = "exp"; }
        else if (tree->content.case_func.math_func_ptr == tan) { funcName = "tan"; }
        else if (tree->content.case_func.math_func_ptr == sqrt) { funcName = "sqrt"; }
        emit parsersOutputSent("\t\t" + funcName, Qt::gray);
        outExprNode(tree->content.case_func.child);
    } break;
    case PLUS: {
        emit parsersOutputSent("\t\t+", Qt::gray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case MINUS: {
        emit parsersOutputSent("\t\t-", Qt::gray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case MUL: {
        emit parsersOutputSent("\t\t*", Qt::gray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case DIV: {
        emit parsersOutputSent("\t\t/", Qt::gray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case POWER: {
        emit parsersOutputSent("\t\t**", Qt::gray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    }
}

struct ExprNode* Parsers::makeExprNode(TokenType op_code, ...) {
    struct ExprNode *expr_ptr = new (struct ExprNode);
    expr_ptr->op_code = op_code;
    va_list arg_ptr;
    va_start(arg_ptr, op_code);
    switch (op_code) {
    case CONST_ID: expr_ptr->content.case_const = (double)va_arg(arg_ptr, double); break;
    case T: expr_ptr->content.case_parm_ptr = &parameter; break;
    case FUNC:
        expr_ptr->content.case_func.math_func_ptr = (FuncPtr)va_arg(arg_ptr, FuncPtr);
        expr_ptr->content.case_func.child = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*); break;
    default:
        expr_ptr->content.case_operator.left = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*);
        expr_ptr->content.case_operator.right = (struct ExprNode*)va_arg(arg_ptr, struct ExprNode*); break;
    }
    va_end(arg_ptr);
    return expr_ptr;
}
void Parsers::fetchToken() {
    temp_token = token_stream[cnt];
    cnt++;
    if (temp_token.type == ERRTOKEN) syntaxError(1);
}
void Parsers::matchToken(TokenType t) {
    if (temp_token.type != t) syntaxError(2);
    fetchToken();
}
void Parsers::syntaxError(int x) {
    if (x == 1) {
        emit parsersOutputSent("line No:" + QString::number(temp_token.token_line) + " Has An Error Token: " + temp_token.lexeme, Qt::red);
        qDebug() << "line No:" << temp_token.token_line << " Has An Error Token: " << temp_token.lexeme;
        exit(0);
    } else if (x == 2) {
        emit parsersOutputSent("line No:" + QString::number(temp_token.token_line) + " Has An Unexpected Token: " + temp_token.lexeme, Qt::red);
        qDebug() << "line No:" << temp_token.token_line << " Has An Unexpected Token: " << temp_token.lexeme;
        exit(0);
    }
}

double Parsers::getExpValue(struct ExprNode* tree) {
    if (tree == nullptr) return 0.0;
    switch (tree->op_code) {
    case PLUS:
        return getExpValue(tree->content.case_operator.left) + getExpValue(tree->content.case_operator.right);
    case MINUS:
        return getExpValue(tree->content.case_operator.left) - getExpValue(tree->content.case_operator.right);
    case MUL:
        return getExpValue(tree->content.case_operator.left) * getExpValue(tree->content.case_operator.right);
    case DIV:
        return getExpValue(tree->content.case_operator.left) / getExpValue(tree->content.case_operator.right);
    case POWER:
        return pow(getExpValue(tree->content.case_operator.left), getExpValue(tree->content.case_operator.right));
    case FUNC:
        return tree->content.case_func.math_func_ptr(getExpValue(tree->content.case_func.child));
    case CONST_ID:
        return tree->content.case_const;
    case T:
        return *(tree->content.case_parm_ptr);
    default:
        return 0.0;
    }
}
void Parsers::parser() {
    fetchToken();
    program();
}
void Parsers::program() {
    emit parsersOutputSent("enter in Program", Qt::black);
    while (temp_token.type != NONTOKEN) {
        emit parsersOutputSent("enter in Parser", Qt::black);
        statement();
        matchToken(SEMICO);
        emit parsersOutputSent("exit from Parser", Qt::black);
    }
    emit parsersOutputSent("exit from Program", Qt::black);
}
void Parsers::statement() {
    emit parsersOutputSent("enter in Statement", Qt::black);
    if (temp_token.type == ORIGIN) originStatement();
    else if (temp_token.type == SCALE) scaleStatement();
    else if (temp_token.type == ROT) rotStatement();
    else if (temp_token.type == FOR) forStatement();
    else if (temp_token.type == COLOR) colorStatement();
    else if (temp_token.type == NOTES) notesStatement();
    else {syntaxError(2);}
    emit parsersOutputSent("exit from Statement", Qt::black);
}

void Parsers::notesStatement() {
    emit parsersOutputSent("enter in NotesStatement", Qt::black);
    temp_notes = "";
    matchToken(NOTES);
    emit parsersOutputSent("\tmatchtoken NOTES", Qt::darkGray);
    matchToken(COLON);
    emit parsersOutputSent("\tmatchtoken :", Qt::darkGray);
    matchToken(L_BRACKET);
    emit parsersOutputSent("\tmatchtoken (", Qt::darkGray);
    notes_x_ptr = expression();
    outExprNode(notes_x_ptr);
    x_notes = getExpValue(notes_x_ptr);
    notes_x_values.push_back(x_notes);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    notes_y_ptr = expression();
    outExprNode(notes_y_ptr);
    y_notes = getExpValue(notes_y_ptr);
    notes_y_values.push_back(y_notes);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    if (temp_token.type != QUOTES) syntaxError(2);
    else {
        temp_token = token_stream[cnt];
        cnt++;
    }
    emit parsersOutputSent("\tmatchtoken \"", Qt::darkGray);
    while (temp_token.type != QUOTES) {
        temp_notes.append(temp_token.lexeme);
        emit parsersOutputSent("\t\t" + temp_token.lexeme, Qt::darkGray);
        temp_notes.append(" ");
        temp_token = token_stream[cnt];
        cnt++;
    }
    notes_string_values.push_back(temp_notes);
    notes_r_values.push_back(r);
    notes_g_values.push_back(g);
    notes_b_values.push_back(b);
    matchToken(QUOTES);
    emit parsersOutputSent("\tmatchtoken \"", Qt::darkGray);
    matchToken(R_BRACKET);
    emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
    emit parsersOutputSent("exit from NotesStatement", Qt::black);
}

void Parsers::colorStatement() {
    emit parsersOutputSent("enter in ColorStatement", Qt::black);
    matchToken(COLOR);
    emit parsersOutputSent("\tmatchtoken COLOR", Qt::darkGray);
    matchToken(IS);
    emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
    matchToken(L_BRACKET);
    emit parsersOutputSent("\tmatchtoken (", Qt::darkGray);
    color_r_ptr = expression();
    outExprNode(color_r_ptr);
    r = getExpValue(color_r_ptr);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    color_g_ptr = expression();
    outExprNode(color_g_ptr);
    g = getExpValue(color_g_ptr);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    color_b_ptr = expression();
    outExprNode(color_b_ptr);
    b = getExpValue(color_b_ptr);
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        emit parsersOutputSent("RGB Color Value Out Of Range(0~255)", Qt::red);
        qDebug() << "RGB Color Value Out Of Range(0~255)";
        exit(0);
    }
    matchToken(R_BRACKET);
    emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
    emit parsersOutputSent("exit from ColorStatement", Qt::black);
}

void Parsers::originStatement() {
    emit parsersOutputSent("enter in OriginStatement", Qt::black);
    matchToken(ORIGIN);
    emit parsersOutputSent("\tmatchtoken ORIGIN", Qt::darkGray);
    matchToken(IS);
    emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
    matchToken(L_BRACKET);
    emit parsersOutputSent("\tmatchtoken (", Qt::darkGray);
    origin_x_ptr = expression();
    outExprNode(origin_x_ptr);
    origin_x = getExpValue(origin_x_ptr);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    origin_y_ptr = expression();
    outExprNode(origin_y_ptr);
    origin_y = getExpValue(origin_y_ptr);
    matchToken(R_BRACKET);
    emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
    emit parsersOutputSent("exit from OriginStatement", Qt::black);
}

void Parsers::rotStatement() {
    emit parsersOutputSent("enter in RotStatement", Qt::black);
    matchToken(ROT);
    emit parsersOutputSent("\tmatchtoken ROT", Qt::darkGray);
    matchToken(IS);
    emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
    rot_ptr = expression();
    outExprNode(rot_ptr);
    rot_ang = getExpValue(rot_ptr);
    emit parsersOutputSent("exit from RotStatement", Qt::black);
}

void Parsers::scaleStatement() {
    emit parsersOutputSent("enter in ScaleStatement", Qt::black);
    matchToken(SCALE);
    emit parsersOutputSent("\tmatchtoken SCALE", Qt::darkGray);
    matchToken(IS);
    emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
    matchToken(L_BRACKET);
    emit parsersOutputSent("\tmatchtoken (", Qt::darkGray);
    scale_x_ptr = expression();
    outExprNode(scale_x_ptr);
    scale_x = getExpValue(scale_x_ptr);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    scale_y_ptr = expression();
    outExprNode(scale_y_ptr);
    scale_y = getExpValue(scale_y_ptr);
    matchToken(R_BRACKET);
    emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
    emit parsersOutputSent("exit from ScaleStatement", Qt::black);
}

void Parsers::forStatement() {
    emit parsersOutputSent("enter in ForStatement", Qt::black);
    matchToken(FOR);
    emit parsersOutputSent("\tmatchtoken FOR", Qt::darkGray);
    matchToken(T);
    emit parsersOutputSent("\tmatchtoken T", Qt::darkGray);
    matchToken(FROM);
    emit parsersOutputSent("\tmatchtoken FROM", Qt::darkGray);
    start_ptr = expression();
    outExprNode(start_ptr);
    start_values.push_back(getExpValue(start_ptr));
    matchToken(TO);
    emit parsersOutputSent("\tmatchtoken TO", Qt::darkGray);
    end_ptr = expression();
    outExprNode(end_ptr);
    end_values.push_back(getExpValue(end_ptr));
    matchToken(STEP);
    emit parsersOutputSent("\tmatchtoken STEP", Qt::darkGray);
    step_ptr = expression();
    outExprNode(step_ptr);
    step_values.push_back(getExpValue(step_ptr));
    matchToken(DRAW);
    emit parsersOutputSent("\tmatchtoken DRAW", Qt::darkGray);
    matchToken(L_BRACKET);
    emit parsersOutputSent("\tmatchtoken (", Qt::darkGray);
    for_x_ptr = expression();
    outExprNode(for_x_ptr);
    for_x_values.push_back(for_x_ptr);
    matchToken(COMMA);
    emit parsersOutputSent("\tmatchtoken ,", Qt::darkGray);
    for_y_ptr = expression();
    outExprNode(for_y_ptr);
    for_y_values.push_back(for_y_ptr);
    origin_x_values.push_back(origin_x);
    origin_y_values.push_back(origin_y);
    scale_x_values.push_back(scale_x);
    scale_y_values.push_back(scale_y);
    rot_ang_values.push_back(rot_ang);
    r_values.push_back(r);
    g_values.push_back(g);
    b_values.push_back(b);
    matchToken(R_BRACKET);
    emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
    emit parsersOutputSent("exit from ForStatement", Qt::black);
}

struct ExprNode* Parsers::expression() {
    struct ExprNode *left, *right;
    TokenType token_tmp;
    left = term();
    while (temp_token.type == PLUS || temp_token.type == MINUS) {
        token_tmp = temp_token.type;
        matchToken(token_tmp);
        right = term();
        left = makeExprNode(token_tmp, left, right);
    }
    return left;
}

struct ExprNode* Parsers::term() {
    struct ExprNode *left, *right;
    TokenType token_tmp;
    left = factor();
    while (temp_token.type == MUL || temp_token.type == DIV) {
        token_tmp = temp_token.type;
        matchToken(token_tmp);
        right = factor();
        left = makeExprNode(token_tmp, left, right);
    }
    return left;
}

struct ExprNode* Parsers::factor() {
    struct ExprNode *left, *right;
    if (temp_token.type == PLUS) {
        matchToken(PLUS);
        right = factor();
        left = nullptr;
        right = makeExprNode(PLUS, left, right);
    } else if (temp_token.type == MINUS) {
        matchToken(MINUS);
        right = factor();
        left = makeExprNode(CONST_ID, 0.0);
        right = makeExprNode(MINUS, left, right);
    } else right = component();
    return right;
}

struct ExprNode* Parsers::component() {
    struct ExprNode *left, *right;
    left = atom();
    if (temp_token.type == POWER) {
        matchToken(POWER);
        right = component();
        left = makeExprNode(POWER, left, right);
    }
    return left;
}

struct ExprNode* Parsers::atom() {
    struct ExprNode *address, *tmp;
    double const_value;
    FuncPtr func_ptr_value;
    if (temp_token.type == CONST_ID) {
        const_value = temp_token.value;
        matchToken(CONST_ID);
        address = makeExprNode(CONST_ID, const_value);
    } else if (temp_token.type == T) {
        matchToken(T);
        address = makeExprNode(T);
    } else if (temp_token.type == FUNC) {
        func_ptr_value = temp_token.FuncPtr;
        matchToken(FUNC);
        matchToken(L_BRACKET);
        tmp = expression();
        address = makeExprNode(FUNC, func_ptr_value, tmp);
        matchToken(R_BRACKET);
    } else if (temp_token.type == L_BRACKET) {
        matchToken(L_BRACKET);
        address = expression();
        matchToken(R_BRACKET);
    } else syntaxError(2);
    return address;
}
