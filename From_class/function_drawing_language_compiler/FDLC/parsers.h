#ifndef PARSERS_H
#define PARSERS_H

#include <QObject>
#include "globaldefines.h"

#include <QDebug>

class Parsers : public QObject
{
    Q_OBJECT
public:
    enum Status{P_PREPARING, P_START, P_NOTES, P_COLOR, P_ORIGIN, P_ROT, P_SCALE, P_FOR, P_SUCCEED};
    explicit Parsers(QObject *parent = nullptr);
    struct ExprNode* makeExprNode(TokenType op_code, ...);
    void fetchToken();
    void matchToken(TokenType t);
    void syntaxError(int x);
    static double getExpValue(struct ExprNode* tree);
    void outExprNode(struct ExprNode* tree);
    void parser();
    void program();
    void statement();
    void notesStatement();
    void colorStatement();
    void originStatement();
    void rotStatement();
    void scaleStatement();
    void forStatement();
    struct ExprNode* expression();
    struct ExprNode* term();
    struct ExprNode* factor();
    struct ExprNode* component();
    struct ExprNode* atom();

public:
    static int cnt;

private:
    struct ExprNode *origin_x_ptr, *origin_y_ptr, *scale_x_ptr, *scale_y_ptr, *rot_ptr, *start_ptr, *end_ptr, *step_ptr, *for_x_ptr, *for_y_ptr, *color_r_ptr, *color_g_ptr, *color_b_ptr, *notes_x_ptr, *notes_y_ptr;
    double origin_x = 0, origin_y = 0, scale_x = 1, scale_y = 1, rot_ang = 0, r = 0, g = 0, b = 0, x_notes, y_notes;
    Tokens temp_token;

signals:
    void parsersStatusSent(Status status);
    void parsersOutputSent(const QString &text, Qt::GlobalColor color);
};

#endif // PARSERS_H
