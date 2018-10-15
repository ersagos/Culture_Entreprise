/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package consoleavion;

//import javafx.scene.control.Alert;
import java.util.Optional;
/*import javafx.scene.control.ButtonType;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.ChoiceDialog;
import javafx.scene.control.Dialog;*/
import java.util.Date;
import java.util.Vector;
import java.util.List;
import java.util.ArrayList;
import javafx.geometry.Insets;
import javafx.scene.control.DatePicker;
//import javafx.scene.control.DialogPane;
import javafx.scene.control.Label;
import javafx.scene.control.ToggleButton;
import javafx.scene.layout.GridPane;

/**
 *@author Jérémy Grall & Alexandre Capbern
 * Un utilitaire qui gère les boites de dialgoue NECESSITE LA DERNIERE VERSION
 * DE JAVA POUR UTILISER LES BOITES DE DIALOGUE
 */
public class AlertUtil {

   /* public static void alert(String title, String header, String info) { // Affiche une simple boite de dialogue
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setHeaderText(header);
        alert.setContentText(info);
        alert.showAndWait();
    }

    public static void error(String title, String header, String info) { // Affiche une boite de dialogue d'erreur
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setHeaderText(header);
        alert.setContentText(info);
        alert.showAndWait();
    }

    public static boolean Confirmation(String title, String header, String info) { // Affiche une boite de dialogue de confirmation et retourne le choix de l'utilisateur sous forme de booléen
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle(title);
        alert.setHeaderText(header);
        alert.setContentText(info);
        Optional<ButtonType> result = alert.showAndWait();
        return (result.get() == ButtonType.OK);
    }

    public static String TextInput(String title, String header, String content) { // Affiche une boite de dialogue de saisie et retourne le resultat (null si annulé)
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle(title);
        dialog.setHeaderText(header);
        dialog.setContentText(content);
        Optional<String> result = dialog.showAndWait();
        if (result.isPresent()) {
            return result.get();
        } else {
            return null;
        }
    }

    public static String TextInput(String title, String header, String content, String hint) { // Affiche une boite de dialogue de saisie deja remplie et retourne le resultat (null si annulé)
        TextInputDialog dialog = new TextInputDialog(hint);
        dialog.setTitle(title);
        dialog.setHeaderText(header);
        dialog.setContentText(content);
        Optional<String> result = dialog.showAndWait();
        if (result.isPresent()) {
            return result.get();
        } else {
            return null;
        }
    }

    public static Integer ChoiceDialog(String title, String header, String content, Vector elements) { // Affiche une boite de dialogue avec une liste de choix pour l'utilisateur et retourne la position qu'a choisi l'utilisateur (null sinon)
        List<String> choices = new ArrayList<>();
        for (int i = 0; i < elements.size(); i++) {
            choices.add(elements.elementAt(i).toString());
        }
        ChoiceDialog<String> dialog = new ChoiceDialog(choices.get(0), choices);
        dialog.setTitle(title);
        dialog.setHeaderText(header);
        dialog.setContentText(content);
        Optional<String> result = dialog.showAndWait();
        if (result.isPresent()) {
            for (int i = 0; i < elements.size(); i++) {
                if (elements.elementAt(i).toString().equals(result.get())) {
                    return i;
                }
            }
        }
        return null;

    }

    public static void DateInput(String title, String header) {
        final DatePicker datepick = new DatePicker();
        Dialog dlg = new Dialog();
        DialogPane pane = new DialogPane();
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(0, 10, 0, 10));
        grid.add(new Label("Entrez une date :"), 0, 0);
        grid.add(datepick, 0, 0);
        pane.setContent(grid);
        dlg.setHeaderText(header);
        dlg.setTitle(title);
        dlg.setDialogPane(pane);
        dlg.showAndWait();

    }*/

}
