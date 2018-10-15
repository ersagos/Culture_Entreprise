/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package consoleavion;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ResourceBundle;
import java.util.Vector;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.concurrent.Task;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Label;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;

/**
 *
 * @author jerem
 */
public class FXMLDocumentController implements Initializable {

    private static final int requete_update = -6;
    private DatagramSocket socket;
    private Semaphore S = new Semaphore(1);
    private String ip;
    private int affiche = 0;
    private long nb_actualisations = 0;
    private static final int port = 5013;
    private Vector<Avion> vectAnterieur = new Vector<Avion>();
    @FXML
    private Label label;

    @FXML
    private TableView TableAvions;


    @Override
    public void initialize(URL url, ResourceBundle rb) {
        try {
            socket = new DatagramSocket();
            socket.setSoTimeout(1000);
            InitialiserTabAvions();
            ip = "localhost";
            if(ip == null){
                System.exit(1);
            }

            Task task = new Task<Void>() {
                @Override
                public Void call() {
                    while (true) {
                        Platform.runLater(new Runnable() {
                            @Override
                            public void run() {
                                    DemandeListeAvion();
                                    Vector<Avion> vect = getListeAvion();
                                    TableAvions.setItems(FXCollections.observableArrayList(vect));
                            }
                        });
                        try {
                            Thread.sleep(2000);
                        } catch (InterruptedException ex) {
                        }

                    }
                }

            };
            Thread th = new Thread(task);
            th.start();
        } catch (SocketException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

    private void InitialiserTabAvions() {
        TableColumn NumVolCol = new TableColumn("Numéro de vol");
        NumVolCol.setCellValueFactory(new PropertyValueFactory("numero_de_vol"));
        TableColumn XCol = new TableColumn("X");
        XCol.setCellValueFactory(new PropertyValueFactory("coord_x"));
        TableColumn YCol = new TableColumn("Y");
        YCol.setCellValueFactory(new PropertyValueFactory("coord_y"));
        TableColumn AltCol = new TableColumn("Altitude");
        AltCol.setCellValueFactory(new PropertyValueFactory("altitude"));
        TableColumn CapCol = new TableColumn("Cap");
        CapCol.setCellValueFactory(new PropertyValueFactory("cap"));
        TableColumn VitCol = new TableColumn("Vitesse");
        VitCol.setCellValueFactory(new PropertyValueFactory("vitesse"));
        TableColumn EtaCol = new TableColumn("Etat");
        EtaCol.setCellValueFactory(new PropertyValueFactory("etat"));
        TableAvions.getColumns().setAll(NumVolCol, XCol, YCol, AltCol, CapCol, VitCol, EtaCol);
    }

    private byte[] extraire(byte[] data, int debut, int fin) {
        int longueur = fin - debut;
        byte[] extrait = new byte[longueur];
        for (int i = 0; i < longueur; i++) {
            extrait[i] = data[i + debut];
        }
        return extrait;
    }

    private void DemandeListeAvion() {
        try {
            InetAddress adr;
            DatagramPacket packet;
            adr = InetAddress.getByName(ip);
            byte[] data = Pivot.toPivot(requete_update);
            packet = new DatagramPacket(data, data.length, adr, port);
            socket.send(packet);
            System.out.println("envoyé !");
        } catch (UnknownHostException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        } catch (SocketException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(FXMLDocumentController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private Vector<Avion> getListeAvion() {
        Vector<Avion> vect = new Vector<Avion>();
        try {
            DatagramPacket packet = new DatagramPacket(new byte[250], 250);
            socket.receive(packet);
            System.out.println(packet.getLength());
            int nb_avions = Pivot.toInt(extraire(packet.getData(), 0, 4));
            for (int i = 0; i < nb_avions; i++) {
                int decalage = 30 * i;
                String numVol = new String(extraire(packet.getData(), 4 + decalage, decalage + 10));
                int x = Pivot.toInt(extraire(packet.getData(), 10 + decalage, 14 + decalage));
                int y = Pivot.toInt(extraire(packet.getData(), 14 + decalage, 18 + decalage));
                int alt = Pivot.toInt(extraire(packet.getData(), 18 + decalage, 22 + decalage));
                int cap = Pivot.toInt(extraire(packet.getData(), 22 + decalage, 26 + decalage));
                int vitesse = Pivot.toInt(extraire(packet.getData(), 26 + decalage, 30 + decalage));
                int crash = Pivot.toInt(extraire(packet.getData(), 30 + decalage, 34 + decalage));
                Avion a = new Avion(numVol, x, y, alt, cap, vitesse, crash);
                vect.addElement(a);
                System.out.println("Avion : " + a.getNumero_de_vol() + " " + a.getCoord_x() + " " + a.getCoord_y() + " " + Pivot.toInt(extraire(packet.getData(), 30 + decalage, 34 + decalage)));
            }

        } catch (SocketTimeoutException ex) {
           

        } catch (IOException ex) {
           
        }
        return vect;
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


