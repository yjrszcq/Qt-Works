#include "parsers.h"

int Parsers::cnt = 0;

Parsers::Parsers(QObject *parent)
    : QObject{parent}
{}

void Parsers::outExprNode(struct ExprNode* tree) {
    switch (tree->op_code) {
    case CONST_ID: emit parsersOutputSent("\t\t" + QString::number(tree->content.case_const), Qt::darkGray); break;
    case T: emit parsersOutputSent("\t\tT", Qt::darkGray); break;
    case FUNC: {
        QString funcName;
        if (tree->content.case_func.math_func_ptr == sin) { funcName = "sin"; }
        else if (tree->content.case_func.math_func_ptr == cos) { funcName = "cos"; }
        else if (tree->content.case_func.math_func_ptr == log) { funcName = "log"; }
        else if (tree->content.case_func.math_func_ptr == exp) { funcName = "exp"; }
        else if (tree->content.case_func.math_func_ptr == tan) { funcName = "tan"; }
        else if (tree->content.case_func.math_func_ptr == sqrt) { funcName = "sqrt"; }
        emit parsersOutputSent("\t\t" + funcName, Qt::darkGray);
        outExprNode(tree->content.case_func.child);
    } break;
    case PLUS: {
        emit parsersOutputSent("\t\t+", Qt::darkGray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case MINUS: {
        emit parsersOutputSent("\t\t-", Qt::darkGray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case MUL: {
        emit parsersOutputSent("\t\t*", Qt::darkGray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case DIV: {
        emit parsersOutputSent("\t\t/", Qt::darkGray);
        outExprNode(tree->content.case_operator.left);
        outExprNode(tree->content.case_operator.right);
    } break;
    case POWER: {
        emit parsersOutputSent("\t\t**", Qt::darkGray);
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
    try{
        temp_token = token_stream.at(cnt);
        cnt++;
        if (temp_token.type == ERRTOKEN){
            syntaxError(1);
        }
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }


}
void Parsers::matchToken(TokenType t) {
    try{
        if (temp_token.type != t){
            syntaxError(2);
        }
        fetchToken();
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}
void Parsers::syntaxError(int x) {
    try{
        if (x == 1) {
            throw std::runtime_error(("line No:" + QString::number(temp_token.token_line) + " Has An Error Token: " + temp_token.lexeme).toStdString());
        } else if (x == 2) {
            throw std::runtime_error(("line No:" + QString::number(temp_token.token_line) + " Has An Unexpected Token: " + temp_token.lexeme).toStdString());
        }
        throw std::runtime_error("Unknown Error");
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
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
    try{
        emit parsersStatusSent(P_START);
        fetchToken();
        program();
        parserOutput();
        emit parsersStatusSent(P_SUCCEED);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

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
    else if (temp_token.type == PIXSIZE) pixsizeStatement();
    else if (temp_token.type == NOTES) notesStatement();
    else {syntaxError(2);}
    emit parsersOutputSent("exit from Statement", Qt::black);
}

void Parsers::notesStatement() {
    try{
        emit parsersStatusSent(P_NOTES);
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
            temp_token = token_stream.at(cnt);
            cnt++;
        }
        emit parsersOutputSent("\tmatchtoken \"", Qt::darkGray);
        while (temp_token.type != QUOTES) {
            temp_notes.append(temp_token.lexeme);
            emit parsersOutputSent("\t\t" + temp_token.lexeme, Qt::darkGray);
            temp_notes.append(" ");
            temp_token = token_stream.at(cnt);
            cnt++;
        }
        notes_string_values.push_back(temp_notes);
        notes_r_values.push_back(r);
        notes_g_values.push_back(g);
        notes_b_values.push_back(b);
        notes_pix_values.push_back(pix);
        matchToken(QUOTES);
        emit parsersOutputSent("\tmatchtoken \"", Qt::darkGray);
        matchToken(R_BRACKET);
        emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
        emit parsersOutputSent("exit from NotesStatement", Qt::black);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}

void Parsers::colorStatement() {
    try{
        emit parsersStatusSent(P_COLOR);
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
            throw std::runtime_error("RGB Color Value Out Of Range(0~255)");
        }
        matchToken(R_BRACKET);
        emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
        emit parsersOutputSent("exit from ColorStatement", Qt::black);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

void Parsers::pixsizeStatement(){
    try{
        emit parsersStatusSent(P_PIXSIZE);
        emit parsersOutputSent("enter in PixsizeStatement", Qt::black);
        matchToken(PIXSIZE);
        emit parsersOutputSent("\tmatchtoken PIXSIZE", Qt::darkGray);
        matchToken(IS);
        emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
        pix_ptr = expression();
        outExprNode(pix_ptr);
        pix = getExpValue(pix_ptr);
        if (pix <= 0) {
            throw std::runtime_error("The Pix Size Must Be Greater Than 0");
        }
        emit parsersOutputSent("exit from PixsizeStatement", Qt::black);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
}

void Parsers::originStatement() {
    try{
        emit parsersStatusSent(P_ORIGIN);
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
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

void Parsers::rotStatement() {
    try{
        emit parsersStatusSent(P_ROT);
        emit parsersOutputSent("enter in RotStatement", Qt::black);
        matchToken(ROT);
        emit parsersOutputSent("\tmatchtoken ROT", Qt::darkGray);
        matchToken(IS);
        emit parsersOutputSent("\tmatchtoken IS", Qt::darkGray);
        rot_ptr = expression();
        outExprNode(rot_ptr);
        rot_ang = getExpValue(rot_ptr);
        emit parsersOutputSent("exit from RotStatement", Qt::black);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

void Parsers::scaleStatement() {
    try{
        emit parsersStatusSent(P_SCALE);
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
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

void Parsers::forStatement() {
    try{
        emit parsersStatusSent(P_FOR);
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
        pix_values.push_back(pix);
        matchToken(R_BRACKET);
        emit parsersOutputSent("\tmatchtoken )", Qt::darkGray);
        emit parsersOutputSent("exit from ForStatement", Qt::black);
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }

}

struct ExprNode* Parsers::expression() {
    struct ExprNode *left, *right;
    TokenType token_tmp;
    try{
        left = term();
        while (temp_token.type == PLUS || temp_token.type == MINUS) {
            token_tmp = temp_token.type;
            matchToken(token_tmp);
            right = term();
            left = makeExprNode(token_tmp, left, right);
        }
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
    return left;
}

struct ExprNode* Parsers::term() {
    struct ExprNode *left, *right;
    TokenType token_tmp;
    try{
        left = factor();
        while (temp_token.type == MUL || temp_token.type == DIV) {
            token_tmp = temp_token.type;
            matchToken(token_tmp);
            right = factor();
            left = makeExprNode(token_tmp, left, right);
        }
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
    return left;
}

struct ExprNode* Parsers::factor() {
    struct ExprNode *left, *right;
    try{
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
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
    return right;
}

struct ExprNode* Parsers::component() {

    struct ExprNode *left, *right;
    try{
        left = atom();
        if (temp_token.type == POWER) {
            matchToken(POWER);
            right = component();
            left = makeExprNode(POWER, left, right);
        }
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
    return left;
}

struct ExprNode* Parsers::atom() {
    struct ExprNode *address = NULL, *tmp;
    double const_value;
    FuncPtr func_ptr_value;
    try{
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
    } catch(const std::exception &e) {
        throw std::runtime_error(std::string(e.what()));
    }
    return address;
}

void Parsers::nodeTotalXY(double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double pix, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y){
    double x, y;
    parameter = start;
    if (step > 0) {
        while (parameter <= end) {
            x = Parsers::getExpValue(for_x);
            y = Parsers::getExpValue(for_y);
            nodeXY(x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b, pix);
            parameter += step;
        }
    } else if (step < 0) {
        while (parameter >= end) {
            x = Parsers::getExpValue(for_x);
            y = Parsers::getExpValue(for_y);
            nodeXY(x, y, origin_x, origin_y, scale_x, scale_y, rot_ang, r, g, b, pix);
            parameter += step;
        }
    }
}

void Parsers::nodeXY(double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double pix){
    double temp_x, temp_y;
    x *= scale_x;
    y *= scale_y;
    temp_x = x;
    temp_y = y;
    x = temp_x * cos(rot_ang) + temp_y * sin(rot_ang);
    y = temp_y * cos(rot_ang) - temp_x * sin(rot_ang);
    x += origin_x;
    y += origin_y;
    struct DrawNode dn = {x, y, r, g, b, pix};
    draw_node.append(dn);
}

void Parsers::outTextXY(double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b, double notes_pix) {
    struct DrawNode dn = {notes_x, notes_y, notes_r, notes_g, notes_b, notes_pix};
    struct DrawText dt = {dn, notes_string};
    draw_text.append(dt);
}


void Parsers::parserOutput(){
    emit parsersStatusSent(P_OUTPUT);
    if(start_values.size() > 0){
        try{
            for (int i = 0; i < start_values.size(); ++i) {
                nodeTotalXY(origin_x_values.at(i), origin_y_values.at(i), scale_x_values.at(i), scale_y_values.at(i), rot_ang_values.at(i), r_values.at(i), g_values.at(i), b_values.at(i), pix_values.at(i), start_values.at(i), end_values.at(i), step_values.at(i), for_x_values.at(i), for_y_values.at(i));
            }
        } catch(const std::exception &e){
            throw std::runtime_error(std::string(e.what()));
        }

    }
    if(notes_string_values.size() > 0){
        try{
            for (int i = 0; i < notes_string_values.size(); ++i) {
                outTextXY(notes_x_values.at(i), notes_y_values.at(i), notes_string_values.at(i), notes_r_values.at(i), notes_g_values.at(i), notes_b_values.at(i), notes_pix_values.at(i));
            }
        } catch(const std::exception &e){
            throw std::runtime_error(std::string(e.what()));
        }
    }
}
