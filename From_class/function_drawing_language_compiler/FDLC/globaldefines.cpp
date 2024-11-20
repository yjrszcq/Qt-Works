#include "globaldefines.h"

QVector<Tokens> token_stream;
QVector<double> r_values;
QVector<double> g_values;
QVector<double> b_values;
QVector<double> origin_x_values;
QVector<double> origin_y_values;
QVector<double> scale_x_values;
QVector<double> scale_y_values;
QVector<double> rot_ang_values;
QVector<double> start_values;
QVector<double> end_values;
QVector<double> step_values;
QVector<struct ExprNode*> for_x_values;
QVector<struct ExprNode*> for_y_values;
QVector<double> notes_x_values;
QVector<double> notes_y_values;
QVector<QString> notes_string_values;
QVector<double> notes_r_values;
QVector<double> notes_g_values;
QVector<double> notes_b_values;
double parameter;
QString temp_notes;
QVector<struct DrawNode> draw_node;
QVector<struct DrawText> draw_text;

void initAll() {
    token_stream.clear();
    r_values.clear();
    g_values.clear();
    origin_x_values.clear();
    origin_y_values.clear();
    scale_x_values.clear();
    scale_y_values.clear();
    rot_ang_values.clear();
    start_values.clear();
    end_values.clear();
    step_values.clear();
    for_x_values.clear();
    for_y_values.clear();
    notes_x_values.clear();
    notes_y_values.clear();
    notes_string_values.clear();
    notes_r_values.clear();
    notes_g_values.clear();
    notes_b_values.clear();
    parameter = 0;
    temp_notes = "";
    draw_node.clear();
    draw_text.clear();
}


