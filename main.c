#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

GtkClipboard    *clipboard;
GtkTextIter     start,
                end;
GtkTextBuffer   *buffer;
GtkWidget       *window,
                *scrolled_window,
                *textview,
                *table,
                *menu_bar,
                *file_menu,
                *file_item,
                *new_item,
                *open_item,
                *save_item,
                *saveas_item,
                *quit_item,
                *edit_menu,
                *edit_item,
                *copy_item,
                *cut_item,
                *paste_item,
                *selectall_item,
                *question_menu,
                *question_item,
                *about_item,
                *char_label;
GError 		    *err        = NULL;
char            *filename   = "";

void check_save_act () {
	if ( strcmp ( gtk_window_get_title ( GTK_WINDOW ( window ) ), "[s_gtk]" ) == 0 ) {
		gtk_widget_set_sensitive ( save_item, FALSE );
	} else {
        gtk_widget_set_sensitive ( save_item, TRUE );
	}
}
void quit ( GtkWidget *window ) {

    GtkWidget *dialog;
    g_print ( ". .  request->close\n" );
    dialog = gtk_message_dialog_new ( GTK_WINDOW ( window ), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure you want to quit ?" );

    switch ( gtk_dialog_run ( GTK_DIALOG( dialog ) ) ) {
                case GTK_RESPONSE_YES :
                    g_print ( ". .  close->done\n" );
                    gtk_main_quit();
                    break;
                case GTK_RESPONSE_NO:
                    g_print ( ". .  not closed\n" );
                    gtk_widget_destroy ( dialog );
                    break;
        }

}
static void destroy( GtkWidget *widget, gpointer data ) {

    quit ( widget );

}
long int file_size(FILE *file) {

	long int file_s;
	fseek(file, 0, SEEK_END); //sposto cursore alla fine
	file_s = ftell(file); //leggo posizione cursore per lunghezza file
	fseek(file, 0, SEEK_SET); //riporto cursore all'inizio
	return file_s;

}
void about_dialog ( GtkWidget *window, gpointer data )  {

    GtkWidget *dialog;
    g_print ( ". .  request->about\n" );
    dialog = gtk_about_dialog_new ();
    gtk_about_dialog_set_name (GTK_ABOUT_DIALOG( dialog ), "S_GTK");
    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG( dialog ), "0.1");
    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG( dialog ), "http://www.danyez.net");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG( dialog ),"danyez.net");
    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG( dialog ), "A simple GTK Editor and Compiler\n\nBy Danyez");

    gtk_dialog_run ( GTK_DIALOG( dialog ) );
    gtk_widget_destroy ( dialog );

}
void char_count ( GtkWidget *window, gpointer data )  {
}
void if_resized ( GtkWidget *window, gpointer data )  {
}
static void new_doc() {

    gtk_text_buffer_get_bounds (buffer, &start, &end);
    gtk_window_set_title ( GTK_WINDOW ( window ), "[s_gtk]" );
    check_save_act ();
    gtk_text_buffer_delete (buffer, &start, &end);


}
static void open_file() {

    FILE *o_file;
    gint filesize;
    gchar *new_title;
    if ( ( o_file = fopen ( filename, "rb" ) ) ) {
        filesize = file_size ( o_file );
        char text[filesize];
        g_print ( "FILESIZE : %d\n", filesize );
        fread ( text, sizeof ( text ), 1, o_file );
        if ( g_utf8_validate ( text, filesize, NULL ) ) {
            gtk_text_buffer_set_text ( buffer, text, filesize );
            new_title = g_strconcat ( "[s_gtk] ", filename, NULL );
            gtk_window_set_title ( GTK_WINDOW ( window ), new_title );
            check_save_act ();
        } else {
            g_print ( "UTF8 ENCODING ERROR.\n" );
        }
    } else {
        g_print ( "OPENING ERROR.\n" );
    }

}
static void file_load() {

    GtkWidget *open_dialog;
    open_dialog = gtk_file_chooser_dialog_new ( "Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL );
    if ( gtk_dialog_run ( GTK_DIALOG ( open_dialog ) ) == GTK_RESPONSE_ACCEPT ) {

        filename = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( open_dialog ) );
        open_file ();
    }
    gtk_widget_destroy ( open_dialog );

}
static void save_file() {

    gchar *text;
	gboolean result;

    gtk_text_buffer_get_bounds (buffer, &start, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	result = g_file_set_contents (filename, text, -1, &err);

	if(result == FALSE ) {
		g_print(". .  [SAVE ERROR]\n");
	}

	else {
		g_print(". .  %s // [SAVED]\n",filename);
	}

}
static void  saveas_file() {

	gboolean result;
	gchar *text;
	gchar *new_title;

    gtk_text_buffer_get_bounds (buffer, &start, &end);

	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    GtkWidget *open_dialog;
    open_dialog = gtk_file_chooser_dialog_new ("Save File", GTK_WINDOW ( window ), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (open_dialog), TRUE);

    if ( gtk_dialog_run ( GTK_DIALOG ( open_dialog ) ) == GTK_RESPONSE_ACCEPT ) {
        filename = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( open_dialog ) );
        result = g_file_set_contents (filename, text, -1, &err);
        if(result == FALSE ) {
			g_print(". .  [SAVE ERROR]\n");
		} else {
			new_title = g_strconcat ( "[s_gtk] - ", filename, NULL );
            gtk_window_set_title ( GTK_WINDOW ( window ), new_title );
			g_print(". .  %s // [SAVED]\n",filename);
		}
    }
    gtk_widget_destroy ( open_dialog );

}
void select_all ( GtkWidget *window, gpointer data )  {

    gtk_text_buffer_get_bounds ( buffer, &start, &end );
    gtk_text_buffer_place_cursor ( buffer, &start );
    gtk_text_buffer_move_mark_by_name ( buffer, "selection_bound", &end );

}

int main ( int argc, char *argv[] ) {

    system("clear");
    gtk_init ( &argc, &argv );

    window          = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    menu_bar        = gtk_menu_bar_new();
    file_menu       = gtk_menu_new();
    file_item       = gtk_menu_item_new_with_label("File");
    new_item        = gtk_menu_item_new_with_label("New");
    open_item       = gtk_menu_item_new_with_label("Open");
    save_item       = gtk_menu_item_new_with_label("Save");
    saveas_item     = gtk_menu_item_new_with_label("Save as ..");
    quit_item       = gtk_menu_item_new_with_label("Quit");
    question_menu   = gtk_menu_new();
    question_item   = gtk_menu_item_new_with_label("?");
    about_item      = gtk_menu_item_new_with_label("About");
    edit_menu       = gtk_menu_new();
    edit_item       = gtk_menu_item_new_with_label("Edit");
    copy_item       = gtk_menu_item_new_with_label("Copy");
    cut_item        = gtk_menu_item_new_with_label("Cut");
    paste_item      = gtk_menu_item_new_with_label("Paste");
    selectall_item  = gtk_menu_item_new_with_label("Select All");
    textview        = gtk_text_view_new ();
    buffer          = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    char_label      = gtk_label_new ("-------------------------------------------");
    table           = gtk_table_new (3,3,FALSE);
    clipboard       = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);


    gtk_widget_set_size_request ( window, 600, 600 );
    gtk_window_set_title ( GTK_WINDOW ( window ), "[s_gtk]" );

    gtk_menu_append ( GTK_MENU ( file_menu ), new_item );
    gtk_menu_append ( GTK_MENU ( file_menu ), open_item );
    gtk_menu_append ( GTK_MENU ( file_menu ), save_item );
    gtk_menu_append ( GTK_MENU ( file_menu ), saveas_item );
    gtk_menu_append ( GTK_MENU ( file_menu ), quit_item );
    gtk_menu_append ( GTK_MENU ( edit_menu ), copy_item );
    gtk_menu_append ( GTK_MENU ( edit_menu ), cut_item );
    gtk_menu_append ( GTK_MENU ( edit_menu ), paste_item );
    gtk_menu_append ( GTK_MENU ( edit_menu ), selectall_item );
    gtk_menu_append ( GTK_MENU ( question_menu ), about_item );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( file_item ), file_menu );
    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( edit_item ), edit_menu );
    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( question_item ), question_menu );

    gtk_menu_bar_append ( GTK_MENU_BAR ( menu_bar ), file_item );
    gtk_menu_bar_append ( GTK_MENU_BAR ( menu_bar ), edit_item );
    gtk_menu_bar_append ( GTK_MENU_BAR ( menu_bar ), question_item );

    gtk_widget_set_size_request ( menu_bar, 600, 20 );
    gtk_widget_set_size_request ( scrolled_window, 600, 560 );
    gtk_widget_set_size_request ( char_label, 600, 20 );
    gtk_container_add ( GTK_CONTAINER ( scrolled_window ), textview );

    gtk_text_view_set_wrap_mode ( GTK_TEXT_VIEW ( textview ), GTK_WRAP_WORD_CHAR );
    gtk_text_view_set_left_margin ( GTK_TEXT_VIEW ( textview ), 2 );
    gtk_text_view_set_right_margin ( GTK_TEXT_VIEW ( textview ), 2 );
    gtk_container_set_border_width ( GTK_CONTAINER ( scrolled_window ), 2 );

    gtk_container_add ( GTK_CONTAINER ( window ), table );
    gtk_table_attach_defaults ( GTK_TABLE ( table ), menu_bar, 0, 3, 0, 1 );
    gtk_table_attach_defaults ( GTK_TABLE ( table ), scrolled_window, 1, 3, 1, 2 );
    gtk_table_attach_defaults ( GTK_TABLE ( table ), char_label, 2, 3, 2, 3 );

    gtk_widget_show_all ( window );

    g_signal_connect ( window, "delete-event", G_CALLBACK (destroy), NULL );
    g_signal_connect ( buffer, "changed", G_CALLBACK (char_count), NULL );

    gtk_signal_connect_object ( GTK_OBJECT ( about_item ), "activate", GTK_SIGNAL_FUNC ( about_dialog ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( new_item ), "activate", GTK_SIGNAL_FUNC ( new_doc ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( open_item ), "activate", GTK_SIGNAL_FUNC ( file_load ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( save_item ), "activate", GTK_SIGNAL_FUNC ( save_file ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( saveas_item ), "activate", GTK_SIGNAL_FUNC ( saveas_file ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( selectall_item ), "activate", GTK_SIGNAL_FUNC ( select_all ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( copy_item ), "activate", GTK_SIGNAL_FUNC ( gtk_text_buffer_copy_clipboard ( buffer, clipboard ); ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( cut_item ), "activate", GTK_SIGNAL_FUNC ( gtk_text_buffer_cut_clipboard ( buffer, clipboard , TRUE); ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( paste_item ), "activate", GTK_SIGNAL_FUNC ( gtk_text_buffer_paste_clipboard ( buffer, clipboard, NULL, TRUE) ), GTK_OBJECT ( window ) );
    gtk_signal_connect_object ( GTK_OBJECT ( quit_item ), "activate", GTK_SIGNAL_FUNC ( destroy ), GTK_OBJECT ( window ) );



    check_save_act ( );

    gtk_main ( );

    return 0;

}
