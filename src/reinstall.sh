sudo make uninstall
make clean
make
sudo make install

echo "Restarting the PostgreSQL server..."
sudo pg_ctlcluster 12 main restart