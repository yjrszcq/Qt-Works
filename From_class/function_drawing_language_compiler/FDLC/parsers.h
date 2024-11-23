#ifndef PARSERS_H
#define PARSERS_H

#include <QObject>
#include "globaldefines.h"

#include <QDebug>

class Parsers : public QObject
{
    Q_OBJECT
public:
    enum Status{P_PREPARING, P_START, P_NOTES, P_COLOR, P_PIXSIZE, P_ORIGIN, P_ROT, P_SCALE, P_FOR, P_OUTPUT, P_SUCCEED};
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
    void pixsizeStatement();
    void originStatement();
    void rotStatement();
    void scaleStatement();
    void forStatement();
    struct ExprNode* expression();
    struct ExprNode* term();
    struct ExprNode* factor();
    struct ExprNode* component();
    struct ExprNode* atom();
    void nodeTotalXY(double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double pix, double start, double end, double step, struct ExprNode* for_x, struct ExprNode* for_y);
    void nodeXY(double x, double y, double origin_x, double origin_y, double scale_x, double scale_y, double rot_ang, double r, double g, double b, double pix);
    void outTextXY(double notes_x, double notes_y, const QString &notes_string, double notes_r, double notes_g, double notes_b, double notes_pix);
    void parserOutput();

public:
    static int cnt;

private:
    struct ExprNode *origin_x_ptr, *origin_y_ptr, *scale_x_ptr, *scale_y_ptr, *rot_ptr, *start_ptr, *end_ptr, *step_ptr, *for_x_ptr, *for_y_ptr, *color_r_ptr, *pix_ptr, *color_g_ptr, *color_b_ptr, *notes_x_ptr, *notes_y_ptr;
    double origin_x = 0, origin_y = 0, scale_x = 1, scale_y = 1, rot_ang = 0, r = 0, g = 0, b = 0, pix = 1, x_notes, y_notes;
    Tokens temp_token;

signals:
    void parsersStatusSent(Status status);
    void parsersOutputSent(const QString &text, Qt::GlobalColor color);
};

#endif // PARSERS_H
