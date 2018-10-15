/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package consoleordreavion;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.Vector;
import java.util.concurrent.Semaphore;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;

/**
 *
 * @author Jérémy
 */
public class FXMLDocumentController implements Initializable {

    private static final int requete_cap = -5;
    private static final int requete_altitude = -4;
    private static final int requete_vitesse = -3;
    private DatagramSocket socket;
    private String ip;
    private static final int port = 5013;

    @FXML
    private TextField TxtValue;

    @FXML
    private TextField TxtNumAvion;

    @FXML
    private ChoiceBox ChoiceInstruction;

    @FXML
    private Button ButValider;

    @FXML
    private void EnvoyerRequete(ActionEvent event) {
        if (TxtNumAvion.getText().isEmpty()) {
            //AlertUtil.alert("Erreur", null, "Veuillez entrer un numéro d'avion");
            return;
        }
        if (TxtValue.getText().isEmpty()) {
            //AlertUtil.alert("Erreur", null, "Veuillez entrer une valeur à modifier");
            return;
        }
        if (ChoiceInstruction.getSelectionModel().getSelectedIndex() == -1) {
            //AlertUtil.alert("Erreur", null, "Veuillez choisir une valeur à modifier");
            return;
        }
        String numAvion = TxtNumAvion.getText();
        if(numAvion.length() != 5){
             //AlertUtil.alert("Erreur", null, "Le numero de vol doit être composé de 5 caractères");
             return;
        }
        numAvion += '\0';
        int choix = ChoiceInstruction.getSelectionModel().getSelectedIndex();
        byte[] value;
        try {
            value = Pivot.toPivot(Integer.parseInt(TxtValue.getText()));
        } catch (NumberFormatException e) {
            //AlertUtil.error("Erreur", null, "Veuillez entrer un entier pour la valeur à modifier");
            return;
        }
        
        byte[] requete;
        switch(choix){
            case 0 : requete = Pivot.toPivot(requete_cap);
            break;
            case 1 : requete = Pivot.toPivot(requete_vitesse);
            break;
            case 2 : requete = Pivot.toPivot(requete_altitude);
            break;
            default : requete = Pivot.toPivot(0);
            break;
        }
        byte[] idAvion = numAvion.getBytes();
        byte[] buffer = new byte[requete.length + value.length + idAvion.length];
        
        for(int i = 0; i< requete.length;i++){
            buffer[i] = requete[i];
        }
        for(int i = 0; i<idAvion.length;i++){
            buffer[i+requete.length] = idAvion[i];
        }
        
        for(int i = 0; i<value.length;i++){
            buffer[i+requete.length + idAvion.length] = value[i];
        }
        
        try {
            socket.send(new DatagramPacket(buffer,buffer.length,InetAddress.getByName(ip),port));
        } catch (IOException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        try {
            socket = new DatagramSocket();
            socket.setSoTimeout(1000);
           // ip = AlertUtil.TextInput("Adresse IP", "Veuillez entrer l'adresse IP du SGCA", null, "localhost");
            ip = "localhost";
            if(ip == null){
                System.exit(1);
            }
            RemplirChoiceInstruction();
            

        } catch (SocketException ex) {
            Logger.getLogger(FXMLDocumentController.class
                    .getName()).log(Level.SEVERE, null, ex);
        }

    }

    private void RemplirChoiceInstruction() {
        Vector<String> vect = new Vector<String>();
        vect.add("Cap");
        vect.add("Vitesse");
        vect.add("Altitude");
        ChoiceInstruction.setItems(FXCollections.observableArrayList(vect));
    }
    @FXML
    private TextField TxtIP; 
    
    @FXML
    public void ChangeIP(ActionEvent event){
        if(!TxtIP.getText().isEmpty()){
            ip = TxtIP.getText();
        }
    }
   

}
