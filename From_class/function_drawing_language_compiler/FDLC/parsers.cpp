#include "parsers.h"

int Parsers::cnt;

Parsers::Parsers(const QVector<Tokens> &token_stream, QObject *parent)
    : QObject{parent}
{
    this->token_stream = token_stream;
}

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
        emit parsersOutputSent("line No:" + QString::number(temp_token.token_line) + " Has An Error Token: " + temp_token.lexeme, Qt::green);
        exit(0);
    } else if (x == 2) {
        emit parsersOutputSent("line No:" + QString::number(temp_token.token_line) + " Has An Unexpected Token: " + temp_token.lexeme, Qt::green);
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
    emit parsersOutputSent("enter in Statement", Qt::black); // Continue replacing qDebug() with signal
    if (temp_token.type == ORIGIN) originStatement();
    else if (temp_token.type == SCALE) scaleStatement();
    else if (temp_token.type == ROT) rotStatement();
    else if (temp_token.type == FOR) forStatement();
    else if (temp_token.type == COLOR) colorStatement();
    else if (temp_token.type == NOTES) notesStatement();
    else syntaxError(2);
    emit parsersOutputSent("exit from Statement", Qt::black);
}

void Parsers::notesStatement() {
    QString temp_notes;
    matchToken(NOTES);
    matchToken(COLON);
    matchToken(L_BRACKET);
    notes_x_ptr = expression();
    x_notes = getExpValue(notes_x_ptr);
    notes_x_values.push_back(x_notes);
    matchToken(COMMA);
    notes_y_ptr = expression();
    y_notes = getExpValue(notes_y_ptr);
    notes_y_values.push_back(y_notes);
    matchToken(COMMA);
    if (temp_token.type != QUOTES) syntaxError(2);
    else {
        temp_token = token_stream[cnt];
        cnt++;
    }
    while (temp_token.type != QUOTES) {
        temp_notes.append(temp_token.lexeme);
        temp_notes.append(" ");
        temp_token = token_stream[cnt];
        cnt++;
    }
    notes_string_values.push_back(temp_notes.trimmed());
    notes_r_values.push_back(r);
    notes_g_values.push_back(g);
    notes_b_values.push_back(b);
    matchToken(QUOTES);
    matchToken(R_BRACKET);
}

void Parsers::colorStatement() {
    matchToken(COLOR);
    matchToken(IS);
    matchToken(L_BRACKET);
    color_r_ptr = expression();
    r = getExpValue(color_r_ptr);
    matchToken(COMMA);
    color_g_ptr = expression();
    g = getExpValue(color_g_ptr);
    matchToken(COMMA);
    color_b_ptr = expression();
    b = getExpValue(color_b_ptr);
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        emit parsersOutputSent("RGB Color Value Out Of Range(0~255)", Qt::red);
        exit(0);
    }
    matchToken(R_BRACKET);
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
