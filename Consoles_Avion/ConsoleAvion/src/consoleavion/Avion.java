package consoleavion;


import java.util.Objects;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author jerem
 */
public class Avion {
    private final SimpleStringProperty numero_de_vol;
    private final SimpleIntegerProperty coord_x;
    private final SimpleIntegerProperty coord_y;
    private final SimpleIntegerProperty altitude;
    private final SimpleIntegerProperty cap;
    private final SimpleIntegerProperty vitesse;
    private final SimpleStringProperty etat;
    
    public Avion(String numero_de_vol, int coord_x, int coord_y, int altitude, int cap, int vitesse,int crash) {
        this.numero_de_vol = new SimpleStringProperty(numero_de_vol);
        this.coord_x = new SimpleIntegerProperty(coord_x);
        this.coord_y = new SimpleIntegerProperty(coord_y);
        this.altitude = new SimpleIntegerProperty(altitude);
        this.cap = new SimpleIntegerProperty(cap);
        this.vitesse = new SimpleIntegerProperty(vitesse);
        if(crash == 1){
            etat = new SimpleStringProperty("Crashé");
        }
        else if(crash == 2){
             etat = new SimpleStringProperty("Connexion perdue");
        }
        else etat = new SimpleStringProperty("En vol");
    }

    public String getNumero_de_vol() {
        return numero_de_vol.get();
    }

    public Integer getCoord_x() {
        return coord_x.get();
    }

    public Integer getCoord_y() {
        return coord_y.get();
    }

    public Integer getAltitude() {
        return altitude.get();
    }

    public Integer getCap() {
        return cap.get();
    }
    
    public String getEtat() {
        return etat.get();
    }
    
    
    public Integer getVitesse() {
        return vitesse.get();
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 97 * hash + Objects.hashCode(this.numero_de_vol);
        return hash;
    }

   /* @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Avion other = (Avion) obj;
        if (!Objects.equals(this.numero_de_vol.get(), other.numero_de_vol.get())) {
            return false;
        }
        return true;
    }*/
    
    
}
